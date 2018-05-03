#include "peer.h"

namespace sml
{
peer::peer(asio::io_context& io_context, udp_layer& udp_layer, const address& addr)
    : io_context_(io_context)
    , timer_(io_context_)
    , current_state_(initial)
    , udp_layer_(udp_layer)
    , addr_(addr)
{
    encrypt_layer_ = boost::make_shared<encrypt_layer>(
        encrypt_layer::algorithm::AES_128, boost::make_shared<std::string>("1234567890123456"));
    CryptoPP::Integer priv("0xed4b008b62dd2563b6406240ba55ee230bda4c1");
    CryptoPP::Integer pub("0x6e794b0ea27adebc21bcbe0c2ecc1d3cd92baabc91f3"
                          "f375020b8ad07220ca20beffbee1d359de88d820ff1b"
                          "daadb868f2fdec148a9bcfd34b49f4a264a3ab8c9051"
                          "44eda1ef5cf645ddade075e63748597128f83f5ec290"
                          "c8b7808a96b4315ddd6cf8cc8f97b938dbfa6b31d3d8"
                          "2df84c31f6ac55e5362ee417538eb34b419c");

    std::cout << "spriv " << std::hex << priv << std::endl;
    std::cout << "spub " << std::hex << pub << std::endl;
    std::unique_ptr<byte[]> priv_blk(new byte[priv.ByteCount()]);
    std::unique_ptr<byte[]> pub_blk(new byte[pub.ByteCount()]);
    priv.Encode(priv_blk.get(), priv.ByteCount());
    pub.Encode(pub_blk.get(), pub.ByteCount());
    dh_key_agreement_.set_static_key_pairs(std::string((const char*)priv_blk.get(), priv.ByteCount()),
        std::string((const char*)pub_blk.get(), pub.ByteCount()));
    peer_spub_ = std::string((const char*)pub_blk.get(), pub.ByteCount());

    send_public_key();
}

shared_ptr<stream> peer::add_stream(id_type id)
{
    stream_map_type::iterator it = stream_map_.find(id);
    if (it != stream_map_.end())
    {
        return nullptr;
    }
    shared_ptr<stream> new_stream = make_shared<stream>(io_context_, *this, id);
    stream_map_.insert(stream_map_type::value_type(id, new_stream));
    return new_stream;
}

void peer::del_stream(id_type id)
{
    size_t ret = stream_map_.erase(id);
    if (ret < 0)
    {
        output_ring.put(make_shared<error_message>("no such stream\n"));
    }
    else if (ret > 1)
    {
        throw unknown_error();
    }
}

shared_ptr<stream> peer::get_stream(id_type id)
{
    stream_map_type::iterator it = stream_map_.find(id);
    if (it != stream_map_.end())
    {
        return it->second;
    }
    else
    {
        return nullptr;
    }
}

void peer::send_public_key()
{
    dh_key_agreement_.generate_ephemeral_key_pairs();
    std::string&& epub = dh_key_agreement_.ephemeral_pub_key();
    to_send_ = datagram::create_public_key(epub);
    send_raw_datagram_with_retransmit(to_send_);
    current_state_ |= sent_public_key;
}

void peer::handle_public_key(shared_ptr<datagram> new_datagram)
{
    if (!(current_state_ & received_public_key))
    {
        std::string peer_epub = new_datagram->payload_;
        dh_key_agreement_.set_peer_pub_key_pairs(peer_spub_, peer_epub);
        current_state_ |= received_public_key;
    }
    send_ack();
}

void peer::send_partial_shared_key() {}

void peer::generate_key_final()
{
    dh_key_agreement_.generate_shared_key();
    key_ = boost::make_shared<std::string>(dh_key_agreement_.shared_key());
    encrypt_layer_ = boost::make_shared<encrypt_layer>(encrypt_layer::algorithm::AES_128, key_);
}

bool peer::peer_handler(shared_ptr<datagram> new_datagram)
{
    switch (new_datagram->type_)
    {
        case datagram::msg_type::ack:
            if (current_state_ & sent_public_key)
            {
                current_state_ |= receved_public_key_ack;
            }
            timer_.cancel();
            break;
        case datagram::msg_type::public_key:
            handle_public_key(new_datagram);
            break;
        case datagram::msg_type::partial_shared_key:
            break;
        default:
            // not interested in this datagram
            return false;
            break;
    }

    if (current_state_ == public_key_exchanged)
    {
        generate_key_final();
        current_state_ |= shared_key_agreement;
    }
    return true;
}

void peer::send_ack()
{
    send_raw_datagram(datagram::create_ack(0, 0));
}

void peer::feed(shared_ptr<std::string> msg)
{
    // convert to datagram
    shared_ptr<datagram> new_datagram = boost::make_shared<datagram>(msg);

    // if peer_handler's interested in it, no need to feed streams
    if (peer_handler(new_datagram))
    {
        return;
    }

    // never feed streams until established
    if (current_state_ != established)
    {
        return;
    }

    // decrypt
    new_datagram->decrypt_payload(*encrypt_layer_);

    if (new_datagram == nullptr)
    {
        // convertion error, drop
        return;
    }
    stream_map_type::iterator it = stream_map_.find(new_datagram->id_);
    if (it == stream_map_.end())
    {
        stream_map_.insert(stream_map_type::value_type(
            new_datagram->id_, make_shared<stream>(stream(io_context_, *this, new_datagram->id_))));
        // send new stream message
        output_ring.put(make_shared<new_stream>(addr_, new_datagram->id_));

        //        it = stream_map_.find(new_datagram->id_);
        //        if (new_stream_handler_.empty())
        //        {
        //            // record new stream id
        //            new_stream_id_.push_back(new_datagram->id_);
        //        }
        //        else
        //        {
        //            stream_handler_type handler = new_stream_handler_[0];
        //            new_stream_handler_.erase(new_stream_handler_.begin());
        //            handler(it->second);
        //        }
    }
    it->second->feed(new_datagram);
}

// void peer::feed(shared_ptr<std::string> encrypted_msg, shared_ptr<address> addr)
//{
//    // decrypt first
//    boost::shared_ptr<std::string> msg = encrypt_layer_->decrypt(encrypted_msg);
//    // convert to datagram
//    shared_ptr<datagram> new_datagram = boost::make_shared<datagram>(msg);
//    if (new_datagram == nullptr)
//    {
//        // convertion error, drop
//        return;
//    }
//    stream_map_type::iterator it = stream_map_.find(new_datagram->id_);
//    if (it == stream_map_.end())
//    {
//        stream_map_.insert(stream_map_type::value_type(
//            new_datagram->id_, make_shared<stream>(stream(new_datagram->id_, bind(&peer::send_datagram, this, _1)))));

//        it = stream_map_.find(new_datagram->id_);
//        if (new_stream_handler_.empty())
//        {
//            // record new stream id
//            new_stream_id_.push_back(new_datagram->id_);
//        }
//        else
//        {
//            stream_handler_type handler = new_stream_handler_[0];
//            new_stream_handler_.erase(new_stream_handler_.begin());
//            handler(it->second);
//        }
//    }
//    it->second->feed(new_datagram);
//}
void peer::send_datagram(shared_ptr<datagram> msg)
{
    // send nothing until key agreement is complete
    if (current_state_ != established)
    {
        return;
    }

    std::cout << std::string(*msg) << std::endl;
    // encrypt
    msg->encrypt_payload(*encrypt_layer_);

    // convert to rawbytes
    shared_ptr<std::string> bytes = shared_ptr<std::string>(*msg);
    // send bytes
    do_send(bytes);
}

void peer::send_raw_datagram(shared_ptr<datagram> msg)
{
    shared_ptr<std::string> bytes = (shared_ptr<std::string>)(*msg);
    do_send(bytes);
}

void peer::send_raw_datagram_with_retransmit(shared_ptr<datagram> msg)
{
    timer_.cancel();

    send_raw_datagram(msg);

    timer_.expires_from_now(boost::posix_time::seconds(1));
    timer_.async_wait(boost::bind(&peer::retransmit_raw_datagram, this, msg));
}
void peer::retransmit_raw_datagram(shared_ptr<datagram> msg)
{
    send_raw_datagram_with_retransmit(msg);
}

void peer::do_send(shared_ptr<std::string> bytes)
{
    // TODO: add handler
    udp_layer_.send_to(
        bytes, make_shared<address>(addr_), [](shared_ptr<std::string> msg, shared_ptr<address> addr) {});
}

const address& peer::addr() const
{
    return addr_;
}
}
