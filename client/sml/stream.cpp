#include "stream.h"

namespace sml {
stream::stream(id_type id, send_callback_type callback) :
    state_(stream_state_type::initial), id_(id), timer(sml_io_context), send_callback_(callback)
{
}
void stream::feed(shared_ptr<datagram> new_datagram)
{
    switch (new_datagram->type_) {
    case datagram::msg_type::abort:
    case datagram::msg_type::data:
    {
        recv_datagram_queue_[new_datagram->offset_] = new_datagram;
        break;
    }
    case datagram::msg_type::acknowledge:
    {
        send_offset_ = new_datagram->offset_;
        send_one_datagram();
        break;
    }
    default:
        // ignore datagram of other types
        return;
    }

    datagram_map_type::iterator it = recv_datagram_queue_.begin();
    if (it != recv_datagram_queue_.end())
    {
        // drain
        while (it->first == recv_offset_)
        {
            (*recv_data_) += it->second->data_;
            recv_offset_ += it->second->offset_;
            it = recv_datagram_queue_.erase(it);
        }
        shared_ptr<datagram> ack = make_shared<datagram>();
        ack->id_ = this->id_;
        ack->offset_ = this->recv_offset_;
        // send ack
        send_callback_(ack);
    }

    it = send_datagram_queue_.begin();
    if (it != send_datagram_queue_.end())
    {
        // clean acknowledged datagrams
        while (it->first < send_offset_)
        {
            it = send_datagram_queue_.erase(it);
        }
    }
}
stream &stream::operator>>(shared_ptr<std::string> data)
{
    (*data) += *recv_data_;
    recv_data_->clear();
    return *this;
}
stream &stream::operator<<(shared_ptr<std::string> data)
{
    (*send_data_) = *data;

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
    }

    send_one_datagram();

    return *this;
}
void stream::send_one_datagram()
{
    if (send_datagram_queue_.empty()) {
        return;
    }
    send_callback_(send_datagram_queue_.begin()->second);
    timer.expires_from_now(posix_time::seconds(1));
    timer.async_wait(bind(&stream::retransmit, this));
}
void stream::retransmit()
{
    send_callback_(send_datagram_queue_.begin()->second);
    timer.expires_from_now(posix_time::seconds(1));
    timer.async_wait(bind(&stream::retransmit, this));
}
}
