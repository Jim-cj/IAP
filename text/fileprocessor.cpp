#include "fileprocessor.h"
#include <QFile>
#include <QDebug>
#include <QTemporaryFile>

FileProcessor::FileProcessor(QObject *parent)
    : QObject(parent)
{
}

//这个函数用于将bin文件分成256字节的数据包，通过packets返回
QVector<QByteArray> FileProcessor::splitFileIntoPackets(const QString &fileName)
{
    QVector<QByteArray> packets;
    QTemporaryFile tempFile;

    if (!tempFile.open()) {
        qWarning() << "Failed to create temporary file";
        return packets;
    }

    // 复制原始文件到临时文件
    QFile originalFile(fileName);//这里originalFile是一个新创建的对象
    if (!originalFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Failed to open original file:" << fileName;
        return packets;
    }

    tempFile.write(originalFile.readAll());
    originalFile.close();

    // 从临时文件读取数据并分割
    if (!tempFile.seek(0)) {
        qWarning() << "Failed to seek in temporary file";
        return packets;
    }

    QByteArray fileData = tempFile.readAll();
    tempFile.close();

    for (qint64 i = 0; i < fileData.size(); i += 256) {
        //剩余的数据量
        //qint64 remaining = fileData.size() - i;
        QByteArray packet = fileData.mid(i, 256);
        if(packet.size()<256)
        {
            QByteArray padding(256 - packet.size()-4, 0x00);
            QByteArray padding1(4, 0xF0);
            packet.append(padding);
            packet.append(padding1);
        }
        packets.append(packet);
    }

    return packets;
}
