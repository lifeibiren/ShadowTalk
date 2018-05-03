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

    send_hello();
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

void peer::feed(shared_ptr<std::string> msg)
{
    // convert to datagram
    shared_ptr<datagram> new_datagram = boost::make_shared<datagram>(msg);
    std::cout<<std::string(*new_datagram)<<std::endl;
    datagram_handler_(new_datagram);
}

void peer::send_partial_shared_key() {}

void peer::generate_shared_key()
{
    dh_key_agreement_.generate_shared_key();
    key_ = boost::make_shared<std::string>(dh_key_agreement_.shared_key());
    encrypt_layer_ = boost::make_shared<encrypt_layer>(encrypt_layer::algorithm::AES_128, key_);
}

//bool peer::peer_handler(shared_ptr<datagram> new_datagram)
//{
//    switch (new_datagram->type_)
//    {
//        case datagram::msg_type::ack:
//            if (current_state_ == sent_hello)
//            {
//                current_state_ |= received_hello_ack;
//            }
//            else if (current_state_ == sent_public_key)
//            {
//                current_state_ |= receved_public_key_ack;
//            }
//            timer_.cancel();
//            break;
//        case datagram::msg_type::hello:

//            break;
//        case datagram::msg_type::public_key:
//            handle_public_key(new_datagram);
//            break;
//        case datagram::msg_type::partial_shared_key:
//            break;
//        default:
//            // not interested in this datagram
//            return false;
//            break;
//    }

//    if (current_state_ == public_key_exchanged)
//    {
//        generate_key_final();
//        current_state_ |= shared_key_agreement;
//    }
//    return true;
//}

void peer::send_ack()
{
    send_raw_datagram(datagram::create_ack(0, 0));
    log<<__PRETTY_FUNCTION__<<"\n";
}

void peer::send_hello()
{
    send_raw_datagram_with_retransmit(datagram::create_hello());
    current_state_ |= sent_hello;
    datagram_handler_ = boost::bind(&peer::hello_handler, this, _1);
    log<<__PRETTY_FUNCTION__<<"\n";
}

void peer::received_hello_handler(shared_ptr<datagram> new_datagram)
{
    current_state_ |= received_hello;
    send_ack();
    log<<__PRETTY_FUNCTION__<<"\n";
}

void peer::received_hello_ack_handler(shared_ptr<datagram> new_datagram)
{
    current_state_ |= received_hello_ack;
    timer_.cancel();
    log<<__PRETTY_FUNCTION__<<"\n";
}

void peer::hello_complete_handler()
{
    send_public_key();
    datagram_handler_ = boost::bind(&peer::public_key_exchange_handler, this, _1);
    log<<__PRETTY_FUNCTION__<<"\n";
}

void peer::send_public_key()
{
    dh_key_agreement_.generate_ephemeral_key_pairs();
    std::string&& epub = dh_key_agreement_.ephemeral_pub_key();
    send_raw_datagram_with_retransmit(datagram::create_public_key(epub));
    current_state_ |= sent_public_key;
    log<<__PRETTY_FUNCTION__<<"\n";
}

void peer::received_public_key_handler(shared_ptr<datagram> new_datagram)
{
    std::string peer_epub = new_datagram->payload_;
    dh_key_agreement_.set_peer_pub_key_pairs(peer_spub_, peer_epub);
    current_state_ |= received_public_key;
    send_ack();
    log<<__PRETTY_FUNCTION__<<"\n";
}

void peer::received_public_key_ack_handler(shared_ptr<datagram> new_datagram)
{
    current_state_ |= received_public_key_ack;
    timer_.cancel();
    log<<__PRETTY_FUNCTION__<<"\n";
}

void peer::public_key_exchange_complete_handler()
{
    generate_shared_key();
    current_state_ |= shared_key_agreement;
    send_echo();
    datagram_handler_ = boost::bind(&peer::echo_handler, this, _1);
    log<<__PRETTY_FUNCTION__<<"\n";
}

void peer::send_echo()
{
    sent_echo_datagram_ = datagram::create_echo("1234567890");
    send_raw_datagram_with_retransmit(sent_echo_datagram_);
    current_state_ |= wait_for_echo;
    log<<__PRETTY_FUNCTION__<<"\n";
}

void peer::received_echo_handler(shared_ptr<datagram> new_datagram)
{
    send_raw_datagram(datagram::create_echo_back(*new_datagram));
    log<<__PRETTY_FUNCTION__<<"\n";
}

void peer::received_echo_back_handler(shared_ptr<datagram> new_datagram)
{
    if (current_state_ & wait_for_echo)
    {
        if (new_datagram->payload_ != sent_echo_datagram_->payload_)
        {
            std::cerr<<"echo mismatch!!!"<<std::endl;
        }
        else
        {
            timer_.cancel();
            current_state_ &= ~wait_for_echo;
        }
    }
    log<<__PRETTY_FUNCTION__<<"\n";
}

void peer::echo_handler(shared_ptr<datagram> new_datagram)
{
    if (new_datagram->type_ == datagram::msg_type::echo)
    {
        received_echo_handler(new_datagram);
    }
    else if (new_datagram->type_ == datagram::msg_type::echo_back)
    {
        received_echo_back_handler(new_datagram);
    }
    else if (new_datagram->type_ == datagram::msg_type::public_key)
    {
        // peer lost ack of last flight
        send_ack();
    }
    else
    {
        // ignore
    }

    // wait for echo is cleared, means we received expected echo
    if (!(current_state_ & wait_for_echo))
    {
        datagram_handler_ = boost::bind(&peer::established_handler, this, _1);
    }
    log<<__PRETTY_FUNCTION__<<"\n";
}

void peer::established_handler(shared_ptr<datagram> new_datagram)
{
        log<<__PRETTY_FUNCTION__<<"\n";
    // never feed streams until established
    if ((current_state_ & established) != established)
    {
        throw unknown_error();
    }

    if (new_datagram->type_ == datagram::msg_type::hello)
    {
        // hello also means reset
//        current_state_ = initial;
//        received_hello_handler(new_datagram);
//        send_hello();
        return;
    }
    else if (new_datagram->type_ == datagram::msg_type::echo)
    {
        // handle echo
//        received_echo_handler(new_datagram);
        return;
    }
    else if (new_datagram->type_ == datagram::msg_type::echo_back)
    {
        // handle echo back
//        received_echo_back_handler(new_datagram);
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
    }
    it->second->feed(new_datagram);
}

void peer::send_datagram(shared_ptr<datagram> msg)
{
        log<<__PRETTY_FUNCTION__<<"\n";
    // send nothing until key agreement is complete
    if ((current_state_ & established) != established)
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
//        log<<__PRETTY_FUNCTION__<<"\n";
    shared_ptr<std::string> bytes = (shared_ptr<std::string>)(*msg);
    do_send(bytes);
}

void peer::send_raw_datagram_with_retransmit(shared_ptr<datagram> msg)
{
    static int i = 0;
    printf("%d\n", i++);
        log<<__PRETTY_FUNCTION__<<"\n";
    timer_.cancel();

    send_raw_datagram(msg);

    timer_.expires_from_now(boost::posix_time::seconds(1));
    timer_.async_wait(boost::bind(&peer::retransmit_raw_datagram, this, msg));
}
void peer::retransmit_raw_datagram(shared_ptr<datagram> msg)
{
//        log<<__PRETTY_FUNCTION__<<"\n";
    send_raw_datagram(msg);

    timer_.expires_from_now(boost::posix_time::seconds(1));
    timer_.async_wait(boost::bind(&peer::retransmit_raw_datagram, this, msg));
}

void peer::do_send(shared_ptr<std::string> bytes)
{
//        log<<__PRETTY_FUNCTION__<<"\n";
    // TODO: add handler
    udp_layer_.send_to(
        bytes, make_shared<address>(addr_), [](shared_ptr<std::string> msg, shared_ptr<address> addr) {});
}

const address& peer::addr() const
{
    return addr_;
}

void peer::hello_handler(shared_ptr<datagram> new_datagram)
{    log<<__PRETTY_FUNCTION__<<"\n";
    if (new_datagram->type_ == datagram::msg_type::hello)
    {
        received_hello_handler(new_datagram);
    }
    else if (new_datagram->type_ == datagram::msg_type::ack)
    {
        received_hello_ack_handler(new_datagram);
    }
    else
    {
        // ignore
        return;
    }

    if ((current_state_ & hello_complete) == hello_complete)
    {
        hello_complete_handler();
    }
}

void peer::public_key_exchange_handler(shared_ptr<datagram> new_datagram)
{
        log<<__PRETTY_FUNCTION__<<"\n";
    if (new_datagram->type_ == datagram::msg_type::public_key)
    {
        received_public_key_handler(new_datagram);
    }
    else if (new_datagram->type_ == datagram::msg_type::ack)
    {
        received_public_key_ack_handler(new_datagram);
    }
    else if (new_datagram->type_ == datagram::msg_type::hello)
    {
        // peer lost ack of hello
        send_ack();
    }
    else
    {
        // ignore
    }

    if ((current_state_ & public_key_exchanged) == public_key_exchanged)
    {
        public_key_exchange_complete_handler();
    }
}
}
