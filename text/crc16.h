#ifndef CRC16_H
#define CRC16_H
#include <QByteArray>

class CRC16_CCITT_FALSE
{
public:
    CRC16_CCITT_FALSE();
    quint16 CRC16(const QByteArray &data);
};

#endif // CRC16_H
