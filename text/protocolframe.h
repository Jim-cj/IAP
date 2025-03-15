#ifndef PROTOCOLFRAME_H
#define PROTOCOLFRAME_H

#include <QByteArray>
//#include <QtEndian>

class ProtocolFrame
{
public:
    enum Command{
        CmdUpgradeA = 0x0001,
        CmdUpgradeB = 0x0002,
        // 可以添加更多命令
    };

    ProtocolFrame();
    ProtocolFrame(Command command,const QByteArray &data,quint16 crc16);
    ~ProtocolFrame();

    bool isValid() const;
    QByteArray toByteArray() const;
    bool fromByteArray(const QByteArray &data);

    Command getCommand() const;
    quint16 getLength() const;
    QByteArray getData() const;

private:
    quint16 command;
    quint16 length;
    QByteArray data;
    quint32 reserved;
    quint16 crc16;
};

#endif // PROTOCOLFRAME_H
