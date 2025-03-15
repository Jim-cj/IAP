#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QEventLoop>
#include <QPlainTextEdit>
#include <QTcpSocket>
QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QEventLoop loop;

private slots:
    void on_Openfile_clicked();

    void on_Sendfile_clicked();

    void on_linkusart_3_clicked();

    void on_dislinkusart_clicked();

    void on_linkserver_clicked();

    void on_dislinkserver_clicked();

    void on_senddata_clicked();
    void on_SerialData_readToRead();

    void on_clear_clicked();
    void mRead_Data_From_Server();
    //void onTimeout();

private:
    Ui::Widget *ui;
    QSerialPort *serialPort;
    QByteArray *buffer;
    QTcpSocket *client;
    //QTimer *ackTimer;
};
#endif // WIDGET_H
