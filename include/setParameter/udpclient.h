#ifndef UDPCLIENT_H
#define UDPCLIENT_H

#include <QObject>
#include <QUdpSocket>
#include <QThread>
#include "include/setParameter/worker.h"

class UDPClient : public QObject
{
    Q_OBJECT


public:
    explicit UDPClient(QObject *parent = nullptr);

    void udp_start(const QHostAddress &localIp, const quint16 &listnerPort, const QHostAddress &remoteIp, const quint16 &remotePort);
    void send_data(const QString &string, const QString &remoteIp, const quint16 &port);
    void send_data(const QByteArray &send_data, const QString &remoteIp, const quint16 &port);
    void udp_stop();
    void read_receive_data(QUdpSocket* socket);
    void process_receive_data(QByteArray &data_buf);
    Worker *m_worker;  //
signals:

    void has_measurement_data(); // 通知另一个线程去处理这个1s的数据包
    void draw_track_data(); //通知mainwindow去绘制track的函数
    //void updateState(QString state, QVariant inNum, QVariant outNum);
public slots:
    void on_receiveResult(const QString &str);
    void on_ready_listen_come(); //监听新的数据
    void on_ready_send_back();  //发送端口返回的数据
    void on_connection_error(QAbstractSocket::SocketError err); //udp连接错误
private:
    QUdpSocket *udpSendSocket = nullptr;
    QUdpSocket *udpListnerSocket = nullptr;
    QThread m_workThread; // new thread to process the receive data



};

#endif // UDPCLIENT_H

