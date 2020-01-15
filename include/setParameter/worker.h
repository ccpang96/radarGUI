#ifndef WORKER_H
#define WORKER_H
#include <QObject>
#include <QMutex>
#include <QThread>
#include <QMutexLocker>
#include "include/radarGUI/track.h"
#include "include/setParameter/signal_to_data.h"
class Worker : public QObject
{
  Q_OBJECT
public:
  explicit Worker(QObject *parent = nullptr);
  QByteArray a_data_package;
  QMutex qMx;
  Track track; //定义一个track对象
  vector<MeasurementPackage> measurement_pack_list_;
signals:
  void resultReady(const QString &str); // 向外界发送结果

public slots:
  void process_measurement_data(); // 耗时操作

};
#endif // WORKER_H

