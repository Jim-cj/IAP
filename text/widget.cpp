#include "widget.h"
#include "ui_widget.h"
#include <iostream>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include "fileprocessor.h"
#include <QDebug>
#include <protocolframe.h>
#include <crc16.h>
#include <QTimer>
//#include <QTextCodec>


Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    this->setLayout(ui->gridLayout);
    serialPort =new QSerialPort(this);
    buffer = new QByteArray;
    client =new QTcpSocket(this);
    connect(serialPort,&QSerialPort::readyRead,this,&Widget::on_SerialData_readToRead);
    connect(client,SIGNAL(readyRead()),this,SLOT(mRead_Data_From_Server()));
    // 连接定时器的 timeout 信号到槽函数 onTimeout
    //connect(ackTimer,SIGNAL( &QTimer::timeout),this,SLOT(onTimeout()));
    QList<QSerialPortInfo> serialList= QSerialPortInfo::availablePorts();

    for(QSerialPortInfo serialInfo:serialList){
        qDebug()<<serialInfo.portName();
        ui->serialport->addItem(serialInfo.portName());
    }

}

Widget::~Widget()
{
    delete ui;
     delete buffer;
}


/*-----------------------------------文件*/
QByteArray fam;

void splitQByteArray(const QByteArray &data, int chunkSize, QVector<QByteArray> &chunks)
{
    int totalSize = data.size();
    int index = 0;

    while (index < totalSize)
    {
        int size = qMin(chunkSize, totalSize - index);
        QByteArray chunk = data.mid(index, size);
        chunks.append(chunk);
        index += size;
    }
}

void Widget::on_Openfile_clicked()
{
       FileProcessor processor;
       ProtocolFrame::Command cmd;
       // 为 ProtocolFrame::Command 定义 qbswap
       switch (ui->comboBox->currentIndex()) {
       case 0:
           cmd= ProtocolFrame::CmdUpgradeA;
           break;
       case 1:
           cmd= ProtocolFrame::CmdUpgradeB;
           break;
       }
            //cmd= ProtocolFrame::CmdUpgradeB;
       CRC16_CCITT_FALSE crc;

    // 弹出文件对话框，让用户选择文件
       QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "", tr("All Files (*);;Text Files (*.txt)"));

       if (fileName.isEmpty()) {
           return;  // 如果用户取消操作，则不进行任何操作
       }

       QVector<QByteArray> packets = processor.splitFileIntoPackets(fileName);

        //将帧直接在这里打包好了后发送出去
       for (int i = 0; i < packets.size(); ++i) {
           qDebug() << "Packet" << i << ":" << packets[i].size() << "bytes";
           //将一组数据打包成一个数据包
           //对数据进行crc16校验
           //将命令（由用户决定，通过上位机进行选择）
           ProtocolFrame frame(cmd,packets[i],crc.CRC16(packets[i]));
           fam.append(frame.toByteArray());
           qDebug() << "packetslenth:" << fam[1600];
           //通讯协议帧完成，通过以太网和串口进行传输
       }
       qDebug() << "Number of packets:" << packets.size();
       qDebug() << "UPPake sent successfully!"<<packets;
}

void Widget::on_Sendfile_clicked()
{
    // 定义升级命令
        /*uint8_t upgrade_cmd = 0x55;*/  // 假设升级命令是 0x55

    // 定义每个分块的大小
       int chunkSize = 266;

       // 存储分块后的数据
       QVector<QByteArray> chunks;

       // 拆分 QByteArray
       splitQByteArray(fam, chunkSize, chunks);
    CRC16_CCITT_FALSE crc;
    if(ui->communication->currentText() == "以太网")
    {
        // 以太网发送数据
        // 发送数据

        for(qint32 i=0;i<fam.size()/chunkSize;i++){
            qint64 bytesWritten = client->write(chunks[i]);
            // 检查发送结果
            if (bytesWritten == fam.size())
            {
                qDebug() << "UPPake sent successfully!";
            }
            else
            {
                qDebug() << "Failed to send UPPake. Bytes written:" << bytesWritten;
                ui->textBrowser->append("Failed to send UPPake. Bytes written: " + QString::number(bytesWritten));
            }
            // 等待数据发送完成
            while (client->bytesToWrite() > 0)
            {
                client->waitForBytesWritten();
            }
            // 再次检查发送结果
            if (client->bytesToWrite() == 0)
            {
                //ui->textBrowser->append("Data sent successfully: "+data);
                qDebug() << "1UPPake sent successfully!";
            }
            else
            {
                qDebug() << "Failed to send UPPake.";
                ui->textBrowser->append("Failed to send UPPake.");
            }
            if (!client->waitForReadyRead(2000)) {
                qDebug() << "No response from server:" << client->errorString();
                    fam = QByteArray();
                return;
            }else
            {
                qDebug() << "发完第!"+(QString)i+"个包";
                QByteArray response=client->readAll();
                if(response.size() == 1){
                    qDebug() << "第!"+(QString)i+"个包，处理完";
                }

            }
        }


    }else
    {
        // 发送升级命令
//            if (serialPort->write((const char *)&upgrade_cmd, sizeof(upgrade_cmd)))
//            {
//                qDebug() << "Upgrade command sent successfully.";
//            } else {
//                QMessageBox::warning(this, "Error", "Failed to send upgrade command!");
//            }

        // 发送数据
        qint64 bytesWritten = serialPort->write(fam);

        // 检查发送结果
        if (bytesWritten == fam.size())
        {
            qDebug() << "2UPPake sent successfully!";
        }
        else
        {
            qDebug() << "Failed to send UPPake. Bytes written:" << bytesWritten;
            ui->textBrowser->append("Failed to send UPPake. Bytes written: " + QString::number(bytesWritten));
        }
        ui->lineEdit_2->clear();
        // 等待数据发送完成
        if (serialPort->waitForBytesWritten())
        {
            qDebug() << "Data sent successfully!";
            ui->textBrowser->append("3UPPake sent successfully: " + QString(fam));
        }
        else
        {
            qDebug() << "Failed to send UPPake.";
            ui->textBrowser->append("Failed to send UPPake.");
        }
    }
    fam = QByteArray();
}

/*---------------------------------------------串口*/
void Widget::on_linkusart_3_clicked()
{

    //读取用户选择的串口信息
    //串口号
    serialPort->setPortName(ui->serialport->currentText());
    //数据位
    serialPort->setDataBits(QSerialPort::DataBits(ui->databits->currentText().toInt()));
    //校验位
    switch (ui->parity->currentIndex()) {
    case 0:
        serialPort->setParity(QSerialPort::NoParity);
        break;
    case 1://奇校验
        serialPort->setParity(QSerialPort::OddParity);
        break;
    case 2://偶校验
        serialPort->setParity(QSerialPort::EvenParity);
        break;
    }
    //波特率
    serialPort->setBaudRate(ui->bundrate->currentText().toInt());
    //停止位，3是1.5
    serialPort->setStopBits(QSerialPort::StopBits(ui->stopbits->currentText().toInt()));
    //流控
    serialPort->setFlowControl(QSerialPort::NoFlowControl);
    //打开串口

    qDebug() << "Baud Rate:" << ui->bundrate->currentText().toInt();
    if(serialPort->isOpen()){
        qDebug("串口已打开");
    }else{
        serialPort->open(QIODevice::ReadWrite);
        qDebug("打开串口");
    }
}

void Widget::on_dislinkusart_clicked()
{
    if(serialPort->isOpen()){
        serialPort->close();
        qDebug("断开串口");
    }

}

void Widget::on_SerialData_readToRead(){
    QByteArray data = serialPort->readAll();
    *buffer += data;

            // 检查是否接收到完整的信息
            while (buffer->contains("\n")) {
                // 找到第一个换行符的位置
                int index = buffer->indexOf("\n");

                // 提取完整的信息
                QByteArray line = buffer->mid(0, index);

                // 清除缓冲区中的已处理数据
                *buffer = buffer->mid(index + 1);

                // 将信息转换为字符串并显示
                QString re = QString(line);
                qDebug() << "获得信息" << re;
                ui->textBrowser->append(re);
            }
}
/*-------------------------------------------以太网*/
void Widget::on_linkserver_clicked()
{
    // 创建事件循环
    QEventLoop loop;


    client->connectToHost(ui->lineEdit_7->text(),ui->lineEdit->text().toInt());
    if(client->state() == QAbstractSocket::ConnectedState){
        qDebug() << "已连接";
        ui->textBrowser->append("已连接到服务器："+ui->lineEdit_7->text());
    }else{
        qDebug() << "正在连接";
        ui->textBrowser->append("正在连接到服务器：");
    }

}

void Widget::on_dislinkserver_clicked()
{
    if (client->state() == QAbstractSocket::ConnectedState) {
                client->disconnectFromHost();
                qDebug() << "断开连接";
                ui->textBrowser->append("正在断开服务器");
            } else {
                qDebug() << "没有连接";
                ui->textBrowser->append("已断开服务器");
            }

}
/*---------------------------------------发送数据*/
void Widget::on_senddata_clicked()
{
//    if(ui->communication->currentText() =="以太网"){
//            client->write(ui->lineEdit_2->text().toStdString().c_str());
//            if (client->bytesToWrite() == 0) {
//                qDebug() << "Data sent successfully!";
//            } else {
//                qDebug() << "Failed to send data.";
//            }



//    }else{
//        const char* sendData = ui->lineEdit_2->text().toStdString().c_str();
//        serialPort->write(sendData);
//        if(serialPort->waitForBytesWritten()){

//            qDebug()<<"发送成功"<<sendData;
//            //ui->textBrowser->append(sendData);
//            }
//         }
    if(ui->communication->currentText() == "以太网")
        {
            // 以太网发送数据
            QString data = ui->lineEdit_2->text();
            QByteArray byteArray = data.toUtf8();

            // 发送数据
            qint64 bytesWritten = client->write(byteArray);

            // 检查发送结果
            if (bytesWritten == byteArray.size())
            {
                qDebug() << "Data sent successfully!";
            }
            else
            {
                qDebug() << "Failed to send data. Bytes written:" << bytesWritten;
                ui->textBrowser->append("Failed to send data. Bytes written: " + QString::number(bytesWritten));
            }

            // 等待数据发送完成
            while (client->bytesToWrite() > 0)
            {
                client->waitForBytesWritten();
            }

            // 再次检查发送结果
            if (client->bytesToWrite() == 0)
            {

                //ui->textBrowser->append("Data sent successfully: "+data);
                qDebug() << "Data sent successfully!";
            }
            else
            {
                qDebug() << "Failed to send data.";
                ui->textBrowser->append("Failed to send data.");
            }
        }
        else
        {
            // 串口发送数据
            QString data = ui->lineEdit_2->text();
            QByteArray byteArray = data.toUtf8();

            // 发送数据
            qint64 bytesWritten = serialPort->write(byteArray);

            // 检查发送结果
            if (bytesWritten == byteArray.size())
            {
                qDebug() << "Data sent successfully!";
            }
            else
            {
                qDebug() << "Failed to send data. Bytes written:" << bytesWritten;
                ui->textBrowser->append("Failed to send data. Bytes written: " + QString::number(bytesWritten));
            }
            ui->lineEdit_2->clear();
            // 等待数据发送完成
            if (serialPort->waitForBytesWritten())
            {
                qDebug() << "Data sent successfully!";
                ui->textBrowser->append("Data sent successfully: " + data);
            }
            else
            {
                qDebug() << "Failed to send data.";
                ui->textBrowser->append("Failed to send data.");
            }
        }
}
/*---------------------------------------清除显示框数据*/
void Widget::on_clear_clicked()
{
    ui->textBrowser->clear();
}
/*--------------------------------------从服务器读取数据*/
void Widget::mRead_Data_From_Server()
{
    QByteArray data=client->readAll();
    if (!data.isEmpty())
    {
        ui->textBrowser->append("服务器向客户端返回数据：\n");
        ui->textBrowser->insertPlainText(data);
        qDebug() << "回显的数据!"<<data;
    }
}


