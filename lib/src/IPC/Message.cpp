#include <string.h>

#include <Lori/IPC/Message.h>

namespace Lori
{
    template <> void Message::Insert<MessageRawDataObject>(uint16_t& pos, const MessageRawDataObject& obj)
    {
        *reinterpret_cast<uint16_t*>(m_data + pos) = obj.second;
        memcpy(&m_data[pos + sizeof(uint16_t)], obj.first, obj.second);
        pos += obj.second + sizeof(uint16_t);
    }

    template <> void Message::Insert<std::string>(uint16_t& pos, const std::string& obj)
    {
        Message::Insert(pos, MessageRawDataObject((uint8_t*)obj.data(), obj.length()));
    }

    template <> uint16_t Message::GetSize<MessageRawDataObject>(const MessageRawDataObject& obj)
    {
        return sizeof(uint16_t) + obj.second;
    }

    template <> uint16_t Message::GetSize<std::string>(const std::string& obj)
    {
        return sizeof(uint16_t) + obj.length();
    }

    template <> long Message::Decode<MessageRawDataObject>(uint16_t& pos, MessageRawDataObject& obj) const
    {
        if (pos + sizeof(uint16_t) > m_size)
            return ErrorDecodeOutOfBounds;

        uint16_t size = *reinterpret_cast<uint16_t*>(m_data + pos);
        pos += sizeof(uint16_t);

        if (pos + size > m_size)
            return ErrorDecodeOutOfBounds;

        obj.first = new uint8_t[size];
        obj.second = size;

        memcpy(obj.first, m_data + pos, size);
        pos += size;

        return 0;
    }

    template <> long Message::Decode<std::string>(uint16_t& pos, std::string& obj) const
    {
        if (pos + sizeof(uint16_t) > m_size)
            return ErrorDecodeOutOfBounds;

        uint16_t size = *reinterpret_cast<uint16_t*>(m_data + pos);
        pos += sizeof(uint16_t);

        if (pos + size > m_size)
            return ErrorDecodeOutOfBounds;

        obj = std::string(reinterpret_cast<const char*>(m_data + pos), size);
        pos += size;

        return 0;
    }
}
