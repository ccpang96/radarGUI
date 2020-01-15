#ifndef INCLUDE_RADARGUI_MAINWINDOW_H
#define INCLUDE_RADARGUI_MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QDateTime>
#include <QTimer>
#include <QPoint>
#include <QRectF>
#include <QPixmap>
#include <QPainter>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include "Eigen/Dense"
#include "QDebug"
#include "QBitmap"
#include <vector>
#include <stdlib.h>
#include "include/radarGUI/track_package.h"
#include "include/radarGUI/show.h"
#include "include/radarGUI/FusionEKF.h"
#include "include/radarGUI/ground_truth_package.h"
#include "include/radarGUI/tools.h"
#include "include/radarGUI/track.h"
#include "include/map/map_data.h"
#include "include/setParameter/worker.h"
#include "include/setParameter/dbf_to_dam.h"
#include "include/setParameter/dbf_to_dam_define.h"
#include "include/setParameter/dbf_to_calib.h"
#include "include/setParameter/udpclient.h"
#include "ui_mainwindow.h"
#include "include/map/base_objects.h"
#include "include/setParameter/psp_tdb_plot_report.h"
#include <QtEndian>

//常量区对象
/**
 * @brief 这三个参数如何设置：
 * 因为圆外还要绘制刻度，所以圆的直径应该小于正方形的变长
 * 还有一个是tab_widget的大小600 *600
 * label_main（expending的）上有一个pixmap的大小应该和tab_widget相等。
 * 圆形的大小：525*525 要比pixmap的边长小75
 *
 */


const int kRangeRadarMap = 900;
const int kRectangleHigh = 700;   //常量对象：整体长方形的高度
const int kRectangleWidth = 1024; //整体长方形的宽度
const int kRoundDiameter = 700 - 75;   //中心显示界面直径

//static QColor background_color(47,79,79);  //设置GUI的背景颜色
static QColor background_color(0,0,0); //深蓝色
using namespace std;





//画雷达地图
void plotChinamap(QPainter &painter, const PolyLine&radar_map);

//画测量值
void plotMeasurements(QPainter &painter, const vector<MeasurementPackage> &measurement_pack_list);
//画航迹信息
void plotTracks(QPainter &painter, const vector<TrackDataOutputPackage> &track_data_output_pack_list);
//画可靠航迹嘻嘻
void plotTrustTracks(QPainter &painter, const vector<TrustTrackPackage>& trust_track_pack_list);
//在类中只是作为友元的声明，在类外还是要重新声明
void check_files(ifstream& in_file, string& in_name,
                 ofstream& out_file, string& out_name);

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

    //friend void  MainInterface(QPainter &painter);

    //friend void drawChina(const China &link, QPainter &painter, qreal lod);

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    vector<MeasurementPackage> getObject1();
    vector<MeasurementPackage> getObject2();
    vector<MeasurementPackage> getObject3();
    //静态界面部分
    void SetGUIStyle();
    void loadMapData();

    //UDP网络通信部分
    void connectInit(); //UDP连接初始化

    std::shared_ptr<MapData>_mapdata;
    PolyLine radar_map;


protected:
     void paintEvent(QPaintEvent *event);



private:

    Ui::MainWindow *ui;
    Tools tools1; //工具类
     //用于绘制地图
    DBFTODAM _dbf_to_dam;
    DBFTOCALIB _dbf_to_calib;
    PSP_TBD_PLOT_REPORT _psp_tdb_plot_report;




    UDPClient _client;
    QUdpSocket *udpSocket;
    void udp_connect();
    void udp_break();
    bool _is_connect;  //
    quint64 _receive_num;  //接收总数
    quint64 _send_num;  //发送总数
    QString _local_ip;
    QString _remote_ip;  //远程IP
    quint16 _remote_port;  //远程端口
    quint16 _local_port;  //本地端口
    Track track2;
    Track track1;

signals:


public slots:

    void DrawScanningGUI();
    void on_set_aline_parameter();  //获取校准参数
    void on_set_work_parameter();  //获取工作参数
    void on_jz_tr_ctrl_changed(const QString &); //校准通道收发开关控制
    void on_start_aline(); //开始校准
    void on_send_DAM(); //发送正常工作参数
    void on_connect_pushButton(); //UDP开始连接
    void on_hand_send();  //手动发送，用于测试UDP是否连接

};

#endif // INCLUDE_RADARGUI_MAINWINDOW_H
