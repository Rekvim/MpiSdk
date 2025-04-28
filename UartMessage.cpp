#include "UartMessage.h"

UartMessage::UartMessage(Command command)
    : m_command(command)
{
}

UartMessage::UartMessage(Command command, const QByteArray &data)
    : m_command(command)
    , m_data(data)
{
}

UartMessage::UartMessage(Command command, quint16 value)
    : m_command(command)
{
    m_data.push_back(static_cast<char>(value >> 8));
    m_data.push_back(static_cast<char>(value & 0xFF));
}

UartMessage::UartMessage(Command command, quint8 value)
    : m_command(command)
{
    m_data.push_back(static_cast<char>(value));
}

UartMessage::UartMessage(const QByteArray &raw)
{
    if (raw.size() < 5 || raw.at(0) != startbyte)
        return;
    quint8 len = static_cast<quint8>(raw.at(1));
    if (len + 4 != static_cast<quint8>(raw.size()))
        return;

    // Распознаём команду
    quint8 cmd = static_cast<quint8>(raw.at(2));
    m_command = static_cast<Command>(cmd);

    // Извлекаем полезные данные
    for (int i = 1; i < len; ++i) {
        m_data.push_back(raw.at(i + 2));
    }
    // Извлекаем CRC (2 байта)
    m_crc.push_back(raw.at(len + 2));
    m_crc.push_back(raw.at(len + 3));
}

QByteArray UartMessage::toByteArray() const
{
    QByteArray result;
    result.push_back(startbyte);
    result.push_back(static_cast<char>(m_data.length() + 1));
    result.push_back(static_cast<char>(static_cast<quint8>(m_command)));
    result.append(m_data);
    // Добавляем либо существующий CRC, либо вычисляем новый
    QByteArray c = m_crc.isEmpty() ? crc16() : m_crc;
    result.append(c);
    return result;
}

bool UartMessage::checkCRC() const
{
    return m_crc == crc16();
}

void UartMessage::addCRC()
{
    m_crc = crc16();
}

// Вычисление CRC16-CCITT для всего сообщения
QByteArray UartMessage::crc16() const
{
    quint16 wCrc = 0xFFFF;
    auto process = [&](quint8 byte) {
        wCrc ^= static_cast<quint16>(byte) << 8;
        for (int i = 0; i < 8; ++i) {
            if (wCrc & 0x8000)
                wCrc = (wCrc << 1) ^ 0x1021;
            else
                wCrc <<= 1;
        }
    };

    process(static_cast<quint8>(startbyte));
    process(static_cast<quint8>(m_data.length() + 1));
    process(static_cast<quint8>(m_command));
    for (auto b : m_data)
        process(static_cast<quint8>(b));

    QByteArray buf;
    buf.push_back(static_cast<char>(wCrc >> 8));
    buf.push_back(static_cast<char>(wCrc & 0xFF));
    return buf;
}

