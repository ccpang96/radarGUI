#include "include/setParameter/udpclient.h"
#include <QMessageBox>
#include <QNetworkInterface>

/**
 * @brief UDPClient::UDPClient
 * @param parent
 * 说明：构造函数
 */
UDPClient::UDPClient(QObject *parent) :
    QObject(parent)
{
    qDebug("%s", __func__);
}


/**
 * @brief UDPClient::udpStart
 * @param localIp
 * @param listnerPort
 * @param remoteIp
 * @param remotePort
 * 说明：  启动UDP服务
 */
void UDPClient::udp_start(const QHostAddress &localIp, const quint16 &listnerPort, const QHostAddress &remoteIp, const quint16 &remotePort)
{
    Q_UNUSED(remoteIp);
    Q_UNUSED(remotePort);
    qDebug("%s", __func__);

    //send
    if(udpSendSocket == nullptr) {
        udpSendSocket = new QUdpSocket(this);
        qDebug() << "create udpSendSocket" <<endl;
        connect(udpSendSocket, SIGNAL(readyRead()), this, SLOT(on_ready_send_back())); //发送端口得到回传的数据
        connect(udpSendSocket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_connection_error(QAbstractSocket::SocketError))); //连接错误
    }

    //receive
    udpListnerSocket = new QUdpSocket(this);
    if(!udpListnerSocket->bind(localIp, listnerPort)) {
        qWarning("NULLcc");
    }
    connect(udpListnerSocket, SIGNAL(readyRead()), this, SLOT(on_ready_listen_come()));
    connect(udpListnerSocket, SIGNAL(error(QAbstractSocket::SocketError)),this,SLOT(on_connection_error(QAbstractSocket::SocketError)));

    //在此处启动数据处理线程
    //qDebug() << "GUI thread is : " << QThread::currentThreadId(); //打印当前线程的ID
    /*
    m_worker = new Worker();
    m_worker->moveToThread(&m_workThread); //将指定的m_worker槽函数在指定线程m_workThread中调用
    connect(this, &UDPClient::has_measurement_data, m_worker, &Worker::process_measurement_data); //process_measurement_data用于进行目标跟踪
    connect(&m_workThread, &QThread::finished, m_worker, &QObject::deleteLater);
    connect(m_worker, &Worker::resultReady, this, &UDPClient::on_receiveResult);
    m_workThread.start(); //启动新的线程
    qDebug() << "new thread is start" <<endl;
    */
}

/**
 * @brief UDPClient::process_receive_data
 * @param data_buf
 * @说明： 处理测量数据的函数
 */
void UDPClient::process_receive_data(QByteArray &data_buf) {

    QMutexLocker lock{&m_worker->qMx}; //在出作用域后，自动对qMx解锁

    if((data_buf.contains('{')) && data_buf.contains('}')) {
      m_worker->a_data_package.clear();
      m_worker->a_data_package.append(data_buf);
      //此处说明：有一个完整的数据包

      emit has_measurement_data(); //通知新的线程去处理


    } else if ((!data_buf.contains('{')) && (m_worker->a_data_package.isNull())) { // 不包括头且现有的数据为空，就不要
        return;
    } else if ((data_buf.contains('{')) && (!data_buf.contains('}'))) { //有头无尾，先清空原有的内容，再附加
        m_worker->a_data_package.clear();
        m_worker->a_data_package.append(data_buf);
    } else if ((!data_buf.contains('{')) && (!data_buf.contains('}')) && (!m_worker->a_data_package.isNull())) { //无头无尾 不为空
        m_worker->a_data_package.append(data_buf);
    } else if ((!data_buf.contains('{')) && (data_buf.contains(('}'))) && (!m_worker->a_data_package.isNull())) { //无头有尾 不为空
        m_worker->a_data_package.append(data_buf);
        //此时说明收到一个完整的数据包

        emit has_measurement_data(); // 通知新的线程去处理

    } else {
        qDebug() << "not find package head and tail" <<endl;
    }

}


/**
 * @brief UDPClient::on_receiveResult
 * @说明： 用于接收工作线程处理好的数据
 */
void UDPClient::on_receiveResult(const QString &str) {


    qDebug() << str <<endl;
    qDebug() << "track complete! " <<endl;
    emit draw_track_data();

}


/**
 * @brief MyUDP::sendData
 * @param string   发送内容
 * @param remoteIp 目标IP地址
 * @param port     目标端口号
 * @说明：发送字符串
 */
void UDPClient::send_data(const QString &string, const QString &remoteIp, const quint16 &port)
{
    Q_UNUSED(remoteIp);
    qDebug("%s %d", __func__, __LINE__);
    if(udpSendSocket == nullptr) {
        qDebug() << "udpSendSocket is null" <<endl;
        return;
    }

    qDebug("%s %d", __func__, __LINE__);
    QByteArray send_data;
    send_data.append(string);
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    foreach (QNetworkInterface interface, interfaceList) {
        QList<QNetworkAddressEntry> entryList = interface.addressEntries();
        foreach(QNetworkAddressEntry entry , entryList) {
            QString str = entry.broadcast().toString();
            if (str != "") {
                qDebug() << "send data's size = " << send_data.size() <<endl;
                udpSendSocket->writeDatagram(send_data, QHostAddress(str), port);
                qDebug() << "send succes" <<endl;
             }

        }
    }
}

/**
 * @brief UDPClient::sendData
 * @param send_data   发送内容
 * @param remoteIp 目标IP地址
 * @param port     目标端口号
 * @说明： 发送的是字节
 */
void UDPClient::send_data(const QByteArray &send_data, const QString &remoteIp, const quint16 &port) {
    if (udpSendSocket == nullptr) {
       qDebug() << "udpSendSocket is null" <<endl;
       return;
    }


    udpSendSocket->writeDatagram(send_data, QHostAddress(remoteIp), port);
    /*
    QString remote_ip2 = QString("192.168.0.160");
    quint16 remote_port2 = (QString("5000")).toUShort();  //远程端口
    udpSendSocket->writeDatagram(send_data, QHostAddress(remote_ip2), remote_port2);
    qDebug() << "send data's size = " << send_data.size() <<endl;
    */
}

/**
 * @brief UDPClient::readySendRead
 * 说明：处理接收到的数据，是发送端口返回的
 */
void UDPClient::on_ready_send_back()
{
    qDebug("%s", __func__);
    read_receive_data(udpSendSocket);
}

/**
 * @brief UDPClient::readyListnerRead
 * 监听读到数据
 */
void UDPClient::on_ready_listen_come()
{
    qDebug("%s", __func__);
    read_receive_data(udpListnerSocket);
}


/**
 * @brief UDPClient::readyRead
 * @param socket
 * 读取数据
 */
void UDPClient::read_receive_data(QUdpSocket* socket)
{
    qDebug("%s", __func__);
    while (socket->hasPendingDatagrams()) { //hasPendingDatagrams返回true时表示至少有一个数据报在等待被读取
        QByteArray data_buffer;  //temp data buffer to save receive data
        data_buffer.resize(socket->pendingDatagramSize());   //pendingDatagramSize为返回第一个在等待读取报文的size，resize函数是把datagram的size归一化到参数size的大小一样

        QHostAddress sender;
        quint16 senderPort;
        socket->readDatagram(data_buffer.data(), data_buffer.size(), &sender, &senderPort);

        qDebug() << "Message from:" << sender.toString();
        qDebug() << "Message port:" << senderPort;
       // qDebug() << "Message: " << data_buffer;

        //取出一个完整的包
        process_receive_data(data_buffer); //处理一下数据，提取一个完整的包
    }
}



/**
 * @brief MyUDP::connection_error
 * @param err
 * 说明：处理网络连接错误
 */
void UDPClient::on_connection_error(QAbstractSocket::SocketError err)
{
    qDebug("%s", __func__);
    switch(err){
    case 0:QMessageBox::critical(nullptr,"connection error","The connection was refused by the peer (or timed out).",QMessageBox::Ok);
           break;
    case 2:QMessageBox::critical(nullptr,"connection error","The host address was not found.",QMessageBox::Ok);
           break;
    case QAbstractSocket::NetworkError:QMessageBox::critical(nullptr,"connection error","An error occurred with the network .",QMessageBox::Ok);
           break;
    case QAbstractSocket::RemoteHostClosedError:
           QMessageBox::critical(nullptr,"connection error","disconnect .",QMessageBox::Ok);
           break;
    default:QMessageBox::critical(nullptr,"connection error","undefine error.",QMessageBox::Ok);
        qDebug()<<"error is ......."<<err;
        break;
    }
}


/**
 * @brief UDPClient::udpStop
 * @param string
 * @param remoteIp
 * @param port
 * 停止UDP服务
 */
void UDPClient::udp_stop()
{
    qDebug("%s", __func__);
    // 关闭发送Socket
    if(udpSendSocket != nullptr) {
        udpSendSocket->close();
        udpSendSocket = nullptr;
    }
    // 关闭Listner
    if(udpListnerSocket != nullptr) {
        udpListnerSocket->close();
        udpListnerSocket = nullptr;
    }

    //网络不连接时，将新线程退出
    m_workThread.quit();
    m_workThread.wait();
}






