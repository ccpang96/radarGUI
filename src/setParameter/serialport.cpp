#include "include/setParameter/serialport.h"
/*
#include <QDebug>

/**
 * @brief SerialPort::SerialPort
 * @param qSerialPortInfo
 * @param qThread
 *
SerialPort::SerialPort(const QSerialPortInfo & qSerialPortInfo, QThread & qThread) : qSerialPortInfo{ qSerialPortInfo }
{
    qSerialPort.setPort(qSerialPortInfo);
    if (!qSerialPort.open(QIODevice::ReadOnly))
    {
        qSerialPort.close();
        throw std::invalid_argument{"Cannot open serial port"};
    }
    //开始设置串口的属性
    qSerialPort.setBaudRate(QSerialPort::Baud115200);
    qSerialPort.setDataBits(QSerialPort::Data8); //数据位数：Data8 Data7 Data6
    qSerialPort.setParity(QSerialPort::OddParity); //奇偶校验：NoParity OddParity EvenParity
    qSerialPort.setStopBits(QSerialPort::TwoStop); //停止位：OneStop TwoStop
    qSerialPort.setFlowControl(QSerialPort::NoFlowControl); //流控制
    connect(&qThread, SIGNAL(started()), this, SLOT(start()));  //将该函数绑定到另一个线程上
    connect(&qSerialPort, SIGNAL(readyRead), this, SLOT(on_receive_data())); //接收数据
}


/**
 * @brief SerialPort::start
 * @ 串口起始函数
 *
void SerialPort::start()
{
    work = true;   //工作
    stop_flag = false; //不停止
    raw_data.clear(); //清空接收数据
    reciving = false;
    sending = false;


    return;
}



void SerialPort::on_send_data(QByteArray &send_data)
{
 if(qSerialPort.isOpen())
     {
        qSerialPort.write(send_data);
     }
}


/**
 * @brief SerialPort::on_receive_data
 * @ 接收数据的函数
 *
void SerialPort::on_receive_data()
{
    while (work)
    {
        QMutexLocker lock{ &qMx };
        if (qSerialPort.waitForReadyRead(50))
        {
            QByteArray recived_data;
            recived_data = qSerialPort.readAll();
            raw_data.append(recived_data);
        }
        if (stop_flag)
        {
            break;
        }
        lock.unlock();
    }
}

/**
 * @brief SerialPort::getPonitData
 * @return
 *
QByteArray SerialPort::getPonitData()
{
    QMutexLocker lock{ &qMx };

    QByteArray data_copy;
    data_copy = raw_data;
    raw_data.clear();

    lock.unlock();

    return data_copy;

}

/**
 * @brief SerialPort::isWorking
 * @return 串口的工作状态
 * @说明：用于返回串口的工作状态
 *
bool SerialPort::isWorking() const
{
    QMutexLocker lock{ &qMx };
    return work;
}


/**
 * @brief SerialPort::isWorking
 * @return 串口的工作状态
 * @说明：用于返回串口的工作状态
 *
bool SerialPort::isSending() const
{
    QMutexLocker lock{ &qMx };
    return sending;
}

/**
 * @brief SerialPort::isReciving
 * @return  返回串口的接收状态
 *
bool SerialPort::isReciving() const
{
    QMutexLocker lock{ &qMx };
    return reciving;
}



/**
 * @brief SerialPort::moveToThread
 * @param qThread
 * @说明：将
 *
void SerialPort::moveToThread(QThread *qThread)
{
    qSerialPort.moveToThread(qThread);
    QObject::moveToThread(qThread);
}



/**
 * @brief SerialPort::stop
 * @说明：关闭串口函数
 *
void SerialPort::stop()
{
    if (qSerialPort.isOpen())
    {
        qSerialPort.close();
    }
}


/**
 * @brief SerialPort::~SerialPort
 * @说明：串口析构函数
 *
SerialPort::~SerialPort()
{
    QMutexLocker lock{ &qMx };
    stop_flag = true;
    stop();
    work = false;
    this->thread()->quit();
    lock.unlock();

    while (isWorking()) {
        continue;
    }
}
*/
