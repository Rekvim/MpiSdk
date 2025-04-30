#ifndef UARTMESSAGE_H
#define UARTMESSAGE_H

#pragma once
#include <QByteArray>

static constexpr char startbyte = 0xAA;

// Протокольные команды для UART-сообщений
enum class Command : quint8 {
    DAC         = 0xC2,
    ADC         = 0xE1,
    SetChADC    = 0x98,
    ADC_OnOff   = 0x48,
    GetVersion  = 0x00,
    TimerArr    = 0x73,
    SetDO       = 0x41,
    GetDO       = 0x42,
    GetDI       = 0x43,
    OK          = 0x25,
    WrongCRC    = 0x66,
    UnknownCode = 0x13
};

// Класс для упаковки и распаковки UART-сообщений с CRC
class UartMessage {
public:
    explicit UartMessage(const QByteArray &raw);
    explicit UartMessage(Command command);

    UartMessage() = default;
    UartMessage(Command command, const QByteArray &data);
    UartMessage(Command command, quint16 value);
    UartMessage(Command command, quint8 value);


    QByteArray toByteArray() const;
    bool checkCRC() const;
    void addCRC();

    Command GetCommand() const { return m_command; }
    QByteArray GetData() const { return m_data; }

private:
    QByteArray crc16() const;

    Command    m_command{ Command::UnknownCode };
    QByteArray m_data;
    QByteArray m_crc;
};

#endif // UARTMESSAGE_H
