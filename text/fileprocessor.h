#ifndef FILEPROCESSOR_H
#define FILEPROCESSOR_H

#include <QObject>
#include <QByteArray>
#include <QVector>

class FileProcessor : public QObject
{
    Q_OBJECT

public:
    explicit FileProcessor(QObject *parent = nullptr);
    QVector<QByteArray> splitFileIntoPackets(const QString &fileName);

signals:

public slots:
};

#endif // FILEPROCESSOR_H
