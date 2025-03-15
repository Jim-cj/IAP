#include "protocolframe.h"
#include "crc16.h"


ProtocolFrame::ProtocolFrame() : command(0), length(0), reserved(0), crc16(0) {}

ProtocolFrame::ProtocolFrame(Command cmd, const QByteArray &d,quint16 crc16) : command(static_cast<quint16>(cmd)), data(d), reserved(0), crc16(crc16)
{

    length = 2+2+data.size()+4+2;

}

ProtocolFrame::~ProtocolFrame() {}


QByteArray ProtocolFrame::toByteArray() const
{


    QByteArray frame;
    frame.append(reinterpret_cast<const char*>(&command), 2);
    frame.append(reinterpret_cast<const char*>(&length), 2);
    frame.append(data);
    frame.append(reinterpret_cast<const char*>(&reserved), 4);
    quint16 crcValue = crc16;
    frame.append(reinterpret_cast<const char*>(&crcValue), 2);
    return frame;
}

//bool ProtocolFrame::fromByteArray(const QByteArray &data)
//{
//    if (data.size() < HeaderSize)
//        return false;

//    command = *reinterpret_cast<const quint16*>(data.constData());
//    length = *reinterpret_cast<const quint16*>(data.constData() + 2);
//    if (data.size() != HeaderSize + length)
//        return false;

//    data = data.mid(4, data.size() - HeaderSize);
//    reserved = *reinterpret_cast<const quint32*>(data.constData() + length);
//    crc = *reinterpret_cast<const quint16*>(data.constData() + length + 4);

//    this->data = data.mid(0, length);
//    this->crc = crc;

//    return true;
//}

//void ProtocolFrame::calculateCRC()
//{
//    QByteArray headerData = QByteArray::fromRawData(reinterpret_cast<const char*>(&command), 2)
//                           + QByteArray::fromRawData(reinterpret_cast<const char*>(&length), 2)
//                           + QByteArray::fromRawData(reinterpret_cast<const char*>(&reserved), 4);
//    crc16 = QCRC16::crc(headerData + data);
//}

ProtocolFrame::Command ProtocolFrame::getCommand() const
{
    return static_cast<Command>(command);
}

quint16 ProtocolFrame::getLength() const
{
    return length;
}

QByteArray ProtocolFrame::getData() const
{
    return data;
}

