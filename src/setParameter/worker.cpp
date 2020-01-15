#include "include/setParameter/worker.h"
#include <QDebug>

Worker::Worker(QObject *parent) : QObject(parent)
{

}

template <class T>
int getArrayLen(T &array)
{
    return sizeof(array) / sizeof(array[0]);
}
//槽函数执行外界的命令
void Worker::process_measurement_data()
{
  qDebug() << "I'm working in thread:" << QThread::currentThreadId(); //打印当前线程的ID
  QMutexLocker lock{&qMx}; //在出作用域后，自动对qMx解锁
  //下面开始对接收到的数据包进行处理
  qDebug()<< "a data package global: " << a_data_package <<endl;

  SIGNALTODATA *signal_to_data = (SIGNALTODATA *)a_data_package.data();
  SIGNALTODATA measurement_package = *signal_to_data;
  quint32 last_time = 0;
  track.number_frame_ += 1;
  track.dt_ = measurement_package.lTimeStampInfo - last_time;
  last_time = measurement_package.lTimeStampInfo;


   MeasurementPackage one_point;
  //将measurement_package移动到track_measurement_pack_list中
  for(int i = 0; i < getArrayLen(measurement_package.Point); i++) {
     one_point.raw_measurements_ <<  measurement_package.Point[i].distance,
                                     measurement_package.Point[i].angle,
                                     measurement_package.Point[i].speed;
       track.measurement_pack_list_.emplace_back(one_point);
  }
  track.track_object();  //进行数据处理
  a_data_package.clear(); //清除变量
  emit resultReady("Hello"); //向外界发送结果
}


