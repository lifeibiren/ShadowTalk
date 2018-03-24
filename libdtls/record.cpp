#include "record.h"

// record::record(boost::shared_ptr<byte_string> bytes) //big endian
//{

//}
// void record::from_bytes(boost::shared_ptr<byte_string> bytes) //big endian
//{

//}
// boost::shared_ptr<byte_string> record::to_bytes() const //big endian
//{

//}
void record::from_bytes(boost::shared_ptr<byte_string> bytes) // big endian
{
    //    assert(bytes->size() >= sizeof(header_type));
    //    header_type *header_ptr = (header_type *)bytes->c_array();

    //    type_ = header_ptr->type;
    //    epoch_ = header_ptr->epoch.value();
    //    sequence_ = header_ptr->sequence.value();
    //    length_ = header_ptr->length.value();
}
// boost::shared_ptr<byte_string> record::to_bytes() const //big endian
//{
//    header_type header;
//    header.type = type_;
//    header.epoch = epoch_;
//    header.sequence = sequence_.value();
//    header.length = length_;
//    return boost::shared_ptr<byte_string>(new byte_string(&header, sizeof(header)));
//}
record::content_type_type record::type() const
{
    return type_;
}
void record::set_type(content_type_type type)
{
    type_ = type;
}
std::uint16_t record::epoch() const
{
    return epoch_;
}
void record::set_epoch(std::uint16_t new_epoch)
{
    epoch_ = new_epoch;
}
uint48_t record::sequence() const
{
    return sequence_.value();
}
void record::set_sequence(uint48_t new_sequence)
{
    sequence_ = new_sequence;
}
std::uint16_t record::length() const
{
    return length_;
}
