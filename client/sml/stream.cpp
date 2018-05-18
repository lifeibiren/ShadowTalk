#include "stream.h"
#include "peer.h"

namespace sml
{
stream::stream(asio::io_context &io_context, peer &a_peer, id_type id)
    : io_context_(io_context)
    , peer_(a_peer)
    //    , state_(stream_state_type::initial)
    , id_(id)
    , timer(io_context_)
    , send_offset_(0)
    , recv_offset_(0)
    , send_length_(0)
    , recv_length_(0)
{}

void stream::feed(shared_ptr<datagram> new_datagram)
{
    switch (new_datagram->type_)
    {
        case datagram::msg_type::abort:
        case datagram::msg_type::data:
        {
            recv_datagram_queue_[new_datagram->offset_] = new_datagram;
            break;
        }
        case datagram::msg_type::data_ack:
        {
            // update offset
            send_offset_ = new_datagram->offset_;
            send_one_datagram();
            break;
        }
        default:
            // ignore datagram of other types
            return;
    }

    datagram_map_type::iterator it = recv_datagram_queue_.begin();
    if (it != recv_datagram_queue_.end() && it->first == recv_offset_)
    {
        // drain
        do
        {
            std::string recv_data_ = it->second->payload_;
            recv_offset_ += it->second->length_;
            it = recv_datagram_queue_.erase(it);

            // send received data into ring
            output_ring.put(boost::make_shared<recv_data>(peer_.addr(), id_, recv_data_));
        } while (it != recv_datagram_queue_.end() && it->first == recv_offset_);

        // send ack
        send_ack();
    }
}

void stream::send(const std::string &data)
{
//    send_data_list_.push_back(data);

//    send_one_piece();
//}

//void stream::send_one_piece()
//{
    // if to-send queue is empty, nothing to do
//    if (send_data_list_.empty())
//    {
//        return;
//    }

//    const std::string &send_data_ = *(send_data_list_.begin());

    // Get the maximum key
    size_t current_max_offset;
    if (!send_datagram_queue_.empty())
    {
        current_max_offset = send_datagram_queue_.rbegin()->first;
    }
    else
    {
        current_max_offset = send_offset_;
    }

    size_t offset = 0;
    while (offset < data.size())
    {
        size_t left_length = data.size() - offset;
        size_t seg_length = left_length > segment_max_ ? segment_max_ : left_length;

        shared_ptr<datagram> new_datagram = boost::make_shared<datagram>();

        new_datagram->type_ = datagram::msg_type::data;
        new_datagram->id_ = this->id_;
        new_datagram->length_ = seg_length;
        new_datagram->offset_ = offset + current_max_offset;
        new_datagram->payload_ = data.substr(offset, seg_length);

        send_datagram_queue_.insert(datagram_map_type::value_type(new_datagram->offset_, new_datagram));

        offset += seg_length;
    }

//    send_data_list_.pop_front();

    send_one_datagram();
}

void stream::send_one_datagram()
{
    // cancel retransmit timer
    timer.cancel();
    // remove datagrams already sent

    for (datagram_map_type::iterator it = send_datagram_queue_.begin();
         it != send_datagram_queue_.end() && it->first < send_offset_;
         )
    {
        it = send_datagram_queue_.erase(it);
    }
    // if all are sent, do nothing
    if (send_datagram_queue_.empty())
    {
//        send_one_piece();
        return;
    }

    // make a copy and send
    shared_ptr<datagram> to_send = send_datagram_queue_.begin()->second;
    shared_ptr<datagram> copy = boost::make_shared<datagram>();
    *copy = *to_send;

    peer_.send_datagram(copy);
    timer.expires_from_now(posix_time::seconds(1));
    timer.async_wait(bind(&stream::retransmit, this, asio::placeholders::error()));
}

void stream::retransmit(const system::error_code &ec)
{
    if (ec != asio::error::operation_aborted)
    {
        send_one_datagram();
    }
}

void stream::send_ack()
{
    shared_ptr<datagram> ack = make_shared<datagram>();
    ack->type_ = datagram::msg_type::data_ack;
    ack->id_ = this->id_;
    ack->offset_ = this->recv_offset_;
    ack->length_ = 0;
    peer_.send_datagram(ack);
}
}
