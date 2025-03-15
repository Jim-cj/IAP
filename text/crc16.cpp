#include <QtGlobal>  // 包含 quint16 类型
#include <crc16.h>


CRC16_CCITT_FALSE::CRC16_CCITT_FALSE()
{
}
// 计算 CRC-16/CCITT-FALSE
quint16 CRC16_CCITT_FALSE::CRC16(const QByteArray &data) {
    quint16 crc = 0xFFFF;  // 初始值
    const quint16 polynomial = 0x1021;  // CRC-16/CCITT-FALSE 多项式

    for (char byte : data) {
        crc ^= (static_cast<quint16>(byte) << 8);  // 将当前字节与 CRC 高字节异或

        for (int i = 0; i < 8; ++i) {
            if (crc & 0x8000) {  // 检查最高位是否为 1
                crc = (crc << 1) ^ polynomial;  // 左移并异或多项式
            } else {
                crc <<= 1;  // 左移
            }
        }
    }
    return crc;
}
