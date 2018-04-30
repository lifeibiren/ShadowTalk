#include "stream.h"

namespace sml
{
stream::stream(id_type id, send_callback_type callback)
    : state_(stream_state_type::initial)
    , id_(id)
    , timer(sml_io_context)
    , send_callback_(callback)
    , recv_data_(make_shared<std::string>())
    , send_offset_(0), recv_offset_(0)
    , send_length_(0), recv_length_(0)
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
        case datagram::msg_type::acknowledge:
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
    if (it != recv_datagram_queue_.end() &&
            it->first == recv_offset_)
    {
        // drain
        do
        {
            (*recv_data_) += it->second->data_;
            recv_offset_ += it->second->length_;
            it = recv_datagram_queue_.erase(it);
        } while (it != recv_datagram_queue_.end() &&
                 it->first == recv_offset_);

        // send ack
        send_ack();
    }
}

stream& stream::operator>>(shared_ptr<std::string> &data)
{
    data = recv_data_;
    recv_data_ = make_shared<std::string>();
    return *this;
}

stream& stream::operator<<(shared_ptr<std::string> &data)
{
    send_data_ = data;

    size_t offset = 0;
    while (offset < send_data_->size())
    {
        size_t left_length = send_data_->size() - offset;
        size_t seg_length = left_length > segment_max_ ? segment_max_ : left_length;

        shared_ptr<datagram> new_datagram = boost::make_shared<datagram>();

        new_datagram->type_ = datagram::msg_type::data;
        new_datagram->id_ = this->id_;
        new_datagram->length_ = seg_length;
        new_datagram->offset_ = offset;
        new_datagram->data_ = send_data_->substr(offset, seg_length);

        send_datagram_queue_.insert(datagram_map_type::value_type(new_datagram->offset_, new_datagram));

        offset += seg_length;
    }

    send_one_datagram();

    return *this;
}

void stream::send_one_datagram()
{
    timer.cancel();
    // remove datagrams already sent
    datagram_map_type::iterator it = send_datagram_queue_.begin();
    if (it != send_datagram_queue_.end() &&
            it->first < send_offset_)
    {
        do
        {
            it = send_datagram_queue_.erase(it);
        } while (it != send_datagram_queue_.end() &&
                 it->first < send_offset_);
    }
    // if all are sent
    if (send_datagram_queue_.empty())
    {
        return;
    }

    shared_ptr<datagram> to_send = send_datagram_queue_.begin()->second;
    send_callback_(to_send);
    timer.expires_from_now(posix_time::seconds(1));
    timer.async_wait(bind(&stream::retransmit, this));
}

void stream::retransmit()
{
    send_one_datagram();
}

void stream::send_ack()
{
    shared_ptr<datagram> ack = make_shared<datagram>();
    ack->type_ = datagram::msg_type::acknowledge;
    ack->id_ = this->id_;
    ack->offset_ = this->recv_offset_;
    send_callback_(ack);
}
}
