#include "peer.h"

namespace sml
{
peer::peer(asio::io_context& io_context, udp_layer& udp_layer, const address& addr)
    : io_context_(io_context)
    , current_state_(0)
    , udp_layer_(udp_layer)
    , addr_(addr)
{
    encrypt_layer_ = boost::make_shared<encrypt_layer>(
        encrypt_layer::algorithm::AES_128, boost::make_shared<std::string>("1234567890123456"));
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

void peer::async_accept_new_stream(stream_handler_type handler)
{
    if (!new_stream_id_.empty())
    {
        stream_map_type::iterator it = stream_map_.find(new_stream_id_[0]);
        new_stream_id_.erase(new_stream_id_.begin());
        if (it != stream_map_.end())
        {
            handler(it->second);
        }
    }
    else
    {
        new_stream_handler_.push_back(handler);
    }
}

std::vector<peer::id_type> peer::new_stream_id_vec() const
{
    return new_stream_id_;
}

// void peer::packet_handler(shared_ptr<std::string> packet, shared_ptr<address> addr)
//{
//    if (current_state_ == established)
//    {
//        feed(packet, addr);
//    }
//    else
//    {
//        handshake(packet, addr);
//    }
//}
void peer::send_public_params() {}

void peer::handle_public_params(shared_ptr<std::string> msg, shared_ptr<address> addr) {}
void peer::send_fake_shared_key() {}
void peer::handle_fake_shared_key(shared_ptr<std::string> msg, shared_ptr<address> addr) {}
void peer::generate_key() {}

void peer::handshake(shared_ptr<std::string> msg, shared_ptr<address> addr)
{
    //    if (!(current_state_ & sent_public_params))
    //    {
    //        handle_public_params(msg, addr);
    //        current_state_ |= sent_public_params;
    //    }
    //    else if (!(current_state_ & received_public_params))
    //    {
    //        current_state_ |= received_public_params;
    //    }
    //    else if (!(current_state_ & sent_fake_shared_key))
    //    {
    //        handle_fake_shared_key(msg, addr);
    //        current_state_ |= sent_fake_shared_key;
    //    }
    //    else if (!(current_state_ & received_fake_shared_key))
    //    {
    //        current_state_ |= received_fake_shared_key;
    //    }
    //    else
    //    {
    //        generate_key();
    //    }
    // handler_ = feed;
    //    encrypt_layer_ = boost::make_shared<encrypt_layer>(encrypt_layer::algorithm::AES_128, key_);
}

void peer::feed(shared_ptr<std::string> encrypted_msg)
{
    // decrypt first
    boost::shared_ptr<std::string> msg = encrypt_layer_->decrypt(encrypted_msg);
    // convert to datagram
    shared_ptr<datagram> new_datagram = boost::make_shared<datagram>(msg);
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

        it = stream_map_.find(new_datagram->id_);
        if (new_stream_handler_.empty())
        {
            // record new stream id
            new_stream_id_.push_back(new_datagram->id_);
        }
        else
        {
            stream_handler_type handler = new_stream_handler_[0];
            new_stream_handler_.erase(new_stream_handler_.begin());
            handler(it->second);
        }
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
    std::cout << std::string(*msg) << std::endl;
    // convert to rawbytes
    shared_ptr<std::string> bytes = shared_ptr<std::string>(*msg);
    // encrypt first
    boost::shared_ptr<std::string> encrypted = encrypt_layer_->encrypt(bytes);
    // TODO: add handler
    udp_layer_.send_to(
        encrypted, make_shared<address>(addr_), [](shared_ptr<std::string> msg, shared_ptr<address> addr) {});
}

const address& peer::addr() const
{
    return addr_;
}
}
