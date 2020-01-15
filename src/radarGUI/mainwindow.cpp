// Copyright (c) 2019 The radarGUI Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "include/radarGUI/mainwindow.h"

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;


/**
 * @brief MainWindow::MainWindow
 * @param parent
 * 说明：主窗口的构造函数：
 */
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    _mapdata(new MapData),
    ui(new Ui::MainWindow),
    _dbf_to_dam(),
    _dbf_to_calib(),
    _psp_tdb_plot_report(),

    track2()
{
    ui->setupUi(this);

    this->loadMapData();   //加载地图
    SetGUIStyle(); //静态界面设置
    //setWindowFlags(Qt::FramelessWindowHint);  //隐藏窗口的标题栏
    //DrawScanningGUI();
    connectInit();  //UDP初始化
    connect(ui->connect_pushButton_, SIGNAL(clicked()), this, SLOT(on_connect_pushButton()));
   // connect(&_client, SIGNAL(draw_track_data()), this, SLOT(on_draw_track()));

    /*
    //校准通道收发开关控制
    connect(ui->jz_tr_ctrl_, SIGNAL(currentIndexChanged(QString)), this, SLOT(on_jz_tr_ctrl_changed(const QString &)));
    //校准分机参数设置确认
    connect(ui->aline_parameter_set_confirm_, SIGNAL(clicked()), this, SLOT(on_set_aline_parameter()));
    //工作状态参数设确认
    connect(ui->work_parameter_set_confirm_, SIGNAL(clicked()), this, SLOT(on_set_work_parameter()));
    //发送校准参数
    connect(ui->start_aline_, SIGNAL(clicked()), this, SLOT(on_start_aline()));
    //发送DAM参数
     connect(ui->DAM_send_, SIGNAL(clicked()), this, SLOT(on_send_DAM()));


    //设置两个指示灯的状态  初始化全部设置成灰色
    const QString led_SheetStyle = "min-width: 16px; min-height: 16px;max-width:16px; max-height: 16px;border-radius: 8px; ";
    ui->send_aline_led_->setStyleSheet(led_SheetStyle);//改成圆形的
    ui->receive_aline_led_->setStyleSheet(led_SheetStyle);//改成圆形的
    QPixmap *p_gray_led = new QPixmap(":/button_gray");
    p_gray_led->scaled(ui->send_aline_led_->size(),Qt::KeepAspectRatio);  //设置pixmap缩放的尺寸
    ui->send_aline_led_->setScaledContents(true); //置label的属性scaledContents，这个属性的作用是允许（禁止）label缩放它的内容充满整个可用的空间
    ui->send_aline_led_->setPixmap(*p_gray_led);
    ui->receive_aline_led_->setScaledContents(true); //置label的属性scaledContents，这个属性的作用是允许（禁止）label缩放它的内容充满整个可用的空间
    ui->receive_aline_led_->setPixmap(*p_gray_led);

    //通道校准时DAM需要用到的参数
    ui->current_work_mode_label_->setStyleSheet("color:#008080 ;");
    ui->dds_tx_en_label_->setStyleSheet("color:#008080;");
    ui->PAM_en_label_->setStyleSheet("color:#008080");
    ui->LAN_en_label_->setStyleSheet("color:#008080 ;");
    ui->rx_bw_sel_label_->setStyleSheet("color:#008080;");
    ui->sw_filter_sel_label_->setStyleSheet("color:#008080");
*/
    //QTimer *qtime = new QTimer(this);
    //connect(qtime,SIGNAL(timeout()),this,SLOT(object_track())); //用中断实现每隔5ms显示一次界面
   //connect(qtime,SIGNAL(timeout()),this,SLOT(send_simulator_data()));
    //qtime->start(5);//5ms 更新一次
   // object_track();




 }




/***********************第一部分：绘制界面*************************/
void MainWindow::loadMapData()
{

    Point radar_location{117.17, 31.52}; //合肥经纬度
    //Point radar_location{110.11, 40.25};
    double max_radar_detection_distance = 500.0; //雷达界面显示的距离 单位是
    this->_mapdata->load();   //获取边界的最大值和最小值经纬度
    this->_mapdata->getChinaMap(); //将地图存储到vector中
    radar_map = this->_mapdata->drawChinafromll(radar_location, max_radar_detection_distance);

}


/**
 * @brief SetGUIStyle
 * 说明：本函数用于设置GUI的外观
 */
void MainWindow::SetGUIStyle() {

    ui->label_main->setScaledContents(true);  //按比例填充满整个label框
    ui->centralWidget->setWindowState(Qt::WindowMaximized);  //窗口最大化显示
    /*
    //设置tab_GUI的样式
    ui->centralWidget->setStyleSheet("QWidget{""background-color: #F8F8FF  ;"  // 背景
                                        "border:1px solid white;"     //边框的颜色
                                        "}"); */

    //设置tab_GUI的样式
    ui->tab_GUI->setStyleSheet("QWidget{""background-color: #000000  ;"  // 背景
                                         "border:0px solid Teal;"     //边框的颜色
                                         "}");
    //设置tab_SET的样式
}



/*********************第二部分：UDP**********************/

void MainWindow::connectInit() {

    _remote_ip = QString("192.168.1.2");
    _local_ip = tools1.getLocalHostIP().toString();
    _remote_port = (QString("2222")).toUShort();  //远程端口
    _local_port =  (QString("1111")).toUShort();  //本地端口

    ui->remoteIP_->setText(_remote_ip);
    ui->localIP_->setText(_local_ip);
    ui->remoteport_->setRange(1024,9999);
    ui->remoteport_->setValue(_remote_port);
    ui->localport_->setRange(1024,9999);
    ui->localport_->setValue(_local_port);
    _is_connect = false;

    ui->state_label_->setText("断");
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::red);
    ui->state_label_->setPalette(pa);
    ui->connect_pushButton_->setText("连接网络");

    ui->localIP_->setEnabled(true);
    ui->remoteIP_->setEnabled(true);
    ui->remoteport_->setEnabled(true);
    ui->localport_->setEnabled(true);
    ui->localIP_->setEnabled(false);
}

/**
 * @brief MainWindow::on_connect_pushButton
 * @state:
 */

void MainWindow::on_connect_pushButton() {
    if (_is_connect) {
        udp_break();
        //QMessageBox::information(nullptr, "Tip", "udp break!!");
    } else {
        //QMessageBox::information(nullptr, "Tip", "udp connect!!");
        udp_connect();
    }
}

void MainWindow::udp_break() {
    _is_connect = false;
    ui->state_label_->setText("断");
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::red);
    ui->state_label_->setPalette(pa);

    ui->connect_pushButton_->setText("连接网络");

    // 使能远程端口，本地端口，远程IP
    ui->remoteIP_->setEnabled(true);
    ui->remoteport_->setEnabled(true);
    ui->localport_->setEnabled(true);
    _client.udp_stop(); //UDP通信停止
}
/**
 * @brief MainWindow::on_udp_connect
 * 说明：连接网络
 */
void MainWindow::udp_connect() {
    qDebug("%s", __func__);
    //获得人修改的IP参数
    _remote_ip = ui->remoteIP_->text();
    _remote_port = ui->remoteport_->text().toUShort();
    _local_port = ui->localport_->text().toUShort();

    _is_connect = true;
    ui->state_label_->setText("通");
    QPalette pa;
    pa.setColor(QPalette::WindowText,Qt::blue);
    ui->state_label_->setPalette(pa);

    ui->connect_pushButton_->setText("断开网络");
    // 禁用远程端口，本地端口，远程IP
    ui->remoteIP_->setEnabled(false);
    ui->remoteport_->setEnabled(false);
    ui->localport_->setEnabled(false);

    connect(ui->hand_send_, SIGNAL(clicked()), this, SLOT(on_hand_send()));
    _client.udp_start(QHostAddress(_local_ip), _local_port, QHostAddress(_remote_ip), _local_port);

}

void MainWindow::on_hand_send() {


    QString ccpang = "hello, i am Patrick!!";

    QByteArray send_data;
    QByteArray dbf_to_dam_byte;
    QByteArray dbf_to_calib_byte;
    dbf_to_dam_byte.append((char*)&_dbf_to_dam, sizeof(_dbf_to_dam));
    dbf_to_calib_byte.append((char*)&_dbf_to_calib, sizeof(_dbf_to_calib));
    send_data.append(dbf_to_calib_byte, dbf_to_calib_byte.size());
    send_data.append(dbf_to_dam_byte, dbf_to_dam_byte.size());
    _client.send_data(send_data, _remote_ip, _remote_port);

    /*
    FILE* fp = fopen("test.bin","wb");
    fwrite(send_data.data(),1,784,fp);
    fclose(fp);
    */


    //提示信息：
   // QMessageBox::information(nullptr, "Tip", "send success!!");
}


/*********************第二部分：获取下发参数**********************/


/******************************************正常工作**************************************************/
/**
 * @brief MainWindow::on_set_work_parameter
 * @响应正常工作按钮的槽函数
 */
void MainWindow::on_set_work_parameter() {


}

/******************************************校准控制**************************************************/

/**
 * @brief MainWindow::on_jz_tr_ctrl_changed
 * @说明：响应校正通道收发开关控制按钮的槽函数
 */
void MainWindow::on_jz_tr_ctrl_changed(const QString &) {


}


/**
 * @brief MainWindow::on_set_aline_parameter
 * @说明： 校准分机参数设置确认
 */
void MainWindow::on_set_aline_parameter() {


}


/**
 * @brief on_start_aline
 * 说明：开始校准
 */
void MainWindow::on_start_aline() {
}

void MainWindow::on_send_DAM() {
}


/***********************第四部分： 目标跟踪********************************************/





/**
 * @brief plotMeasurements
 * @param painter
 * @param measurement_pack_list
 * 说明:将测量值在GUI上打印出来
 */
void plotMeasurements(QPainter &painter, const vector<MeasurementPackage> &measurement_pack_list) {

    int x;
    int y;
    VectorXd  measurementGUI; //临时变量，存放转换到GUI上的坐标信息
    QColor qcolor_round(255,255,0);  //点的颜色 黄色
    painter.setPen(QPen(qcolor_round, 10, Qt::DotLine, Qt::RoundCap));
    size_t N = measurement_pack_list.size();
    if (N > 0) {
        for (size_t k = 0; k < N; ++k) {
            measurementGUI = tools1.RadarPolarToGUI(measurement_pack_list[k].raw_measurements_);
            x = static_cast<int>(measurementGUI(0));
            y = static_cast<int>(measurementGUI(1));
            painter.drawPoint(x,y);
        }
    }
}



/**
 * @brief plotTracks;
 * @param painter
 * @param measurementGUI_list
 * 说明：将航迹信息track_data_output_pack_list在GUI上画出来 航迹信息中可不止一条航迹
 */
void plotTracks(QPainter &painter, const vector<TrackDataOutputPackage> &track_data_output_pack_list)
{

    int x;
    int y;
    VectorXd  trackGUI; //临时变量，存放转换到GUI上的坐标信息
    QColor qcolor_round_shi(220,20,60);  //实点的颜色 红色
    painter.setPen(QPen(qcolor_round_shi, 2, Qt::DashDotLine, Qt::RoundCap));

    size_t N = track_data_output_pack_list.size();
    for (size_t n = 0; n < N; ++n) {
        if(track_data_output_pack_list[n].raw_track_data_output_(3) == 0) { //说明是实点
            trackGUI = tools1.RadarPolarToGUI(track_data_output_pack_list[n].track_dataout_point_);
            x = static_cast<int>(trackGUI(0));
            y = static_cast<int>(trackGUI(1));
            painter.drawPoint(x,y);
        } else {
            QColor qcolor_round_bu(0,0,255);  //补点的颜色 蓝色
            painter.setPen(QPen(qcolor_round_bu, 2, Qt::DashDotLine, Qt::RoundCap));
            trackGUI = tools1.RadarPolarToGUI(track_data_output_pack_list[n].track_dataout_point_);
            x = static_cast<int>(trackGUI(0));
            y = static_cast<int>(trackGUI(1));
            painter.drawPoint(x,y);
        }

    }
}



/**
 * @brief plotTracks;
 * @param painter
 * @param measurementGUI_list
 * 说明：将航迹信息track_data_output_pack_list在GUI上画出来 航迹信息中可不止一条航迹
 */
void plotTrustTracks(QPainter &painter, const vector<TrustTrackPackage>& trust_track_pack_list)
{

    int x;
    int y;
    VectorXd  trackGUI; //临时变量，存放转换到GUI上的坐标信息
    QColor qcolor_round_shi(220,20,60);  //实点的颜色
    painter.setPen(QPen(qcolor_round_shi, 6, Qt::DashDotLine, Qt::RoundCap));

    size_t N = trust_track_pack_list.size();
    for (size_t n = 0; n < N; ++n) {
        if(trust_track_pack_list[n].raw_track_data_output_(3) == 0) { //说明是实点
            trackGUI = tools1.RadarPolarToGUI(tools1.RadarCartesianToPolar(trust_track_pack_list[n].last_point_x_));
            x = static_cast<int>(trackGUI(0));
            y = static_cast<int>(trackGUI(1));
            painter.drawPoint(x,y);
        } else {
            QColor qcolor_round_bu(255,255,0);  //补点的颜色
            painter.setPen(QPen(qcolor_round_bu, 6, Qt::DashDotLine, Qt::RoundCap));
            trackGUI = tools1.RadarPolarToGUI(tools1.RadarCartesianToPolar(trust_track_pack_list[n].last_point_x_));
            x = static_cast<int>(trackGUI(0));
            y = static_cast<int>(trackGUI(1));
            painter.drawPoint(x,y);
        }

    }
}

/**
 * @brief plotChinamap
 * @param painter
 * @param radar_map
 */
void plotChinamap(QPainter &painter, const PolyLine &radar_map) {

    QColor qcolor_round_shi(148,0,211);  //实点的颜色
    painter.setPen(QPen(qcolor_round_shi, 1, Qt::DashDotLine, Qt::RoundCap));

    for (auto &part : radar_map.parts) {
        QPointF p1(part[0].x, -part[0].y);
        for (auto it = part.begin() + 1; it != part.end()  ; ++it) {

            //qDebug() << it->x << -it->y<< endl;
            QPointF p2(it->x, -it->y);
            painter.drawLine(p1, p2);
            p1 = std::move(p2);
        }
    }


/*
    int x;
    int y;
    double x1;
    double y1;
    for (size_t i = 0; i < N; ++i) {
        x1 = radar_map[i].x;
        y1 = radar_map[i].y;
        if(x1 >= 0 && y1 >= 0) { //第一象限
            y1 = -y1;
        } else if( x1 >= 0 && y1 <0) { //第四象限
            y1 = -y1;
        } else if( x1 < 0 && y1 < 0) { //第三象限
            y1 = -y1;
        } else if(x1 <0 && y1 >= 0){
            y1 = -y1;
        }
        x = static_cast<int>(x1);
        y = static_cast<int>(y1);

       // QPointF p2(x,y);
       // painter.drawLine(p1, p2);
        painter.drawPoint(x,y);
       // p1 = std::move(p2);
    }
   */

}

/**
 * @brief MainWindow::DrawScanningGUI
 * 说明：绘制界面的主要函数
 */
void MainWindow::DrawScanningGUI() {

    QPixmap pixmap(kRectangleWidth,kRectangleHigh);
    pixmap.fill(background_color);  //设置背景颜色
    QPainter painter(&pixmap);


    MainInterface(painter); //静态界面

    //地图在此显示
    plotChinamap(painter,radar_map);


    /**  @brief:雷达的测量值在此显示：  */
    //先将画布移动到圆心
   // painter.translate(QPoint((kRectangleWidth-kRoundDiameter)/2+kRoundDiameter/2,(kRectangleHigh - kRoundDiameter)/2+kRoundDiameter/2));
    //plotMeasurements(painter, measurement_pack_list);

    /** @brief 最终的航迹信息在此显示  */
   // plotTracks(painter, track_data_output_pack_list);
    /** @brief:  动态扫描的部分在此显示 */


    /*
    static double MyScanLine ;
    MyScanLine += 2.0;
    if (MyScanLine >= 360.0) {
        MyScanLine = 0;
    }
    painter.translate(-kRoundDiameter/2,-kRoundDiameter/2);
    QConicalGradient gradient(kRoundDiameter/2,kRoundDiameter/2, MyScanLine);
    gradient.setColorAt(0.2,QColor(47,79,79, 100));
    gradient.setColorAt(0.5,QColor(47,79,79, 0));
    painter.setBrush(gradient);
    painter.setPen(QPen(Qt::NoPen));
    painter.drawPie(0, 0, kRoundDiameter,kRoundDiameter, MyScanLine * 16.0, 60* 16);

    */
    //画图结束
    painter.end();
    pixmap.scaled(ui->label_main->size(), Qt::KeepAspectRatio);
    ui->label_main->setPixmap(pixmap);
    qDebug() << track2.measurement_pack_list_.size()<< endl;
}

void MainWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPixmap pixmap(kRectangleWidth,kRectangleHigh);
    pixmap.fill(background_color);  //设置背景颜色
    QPainter painter(&pixmap);


    MainInterface(painter); //静态界面

    // @brief: 雷达地图在此显示
    plotChinamap(painter,radar_map);


    //  @brief:雷达的测量值在此显示：

    //plotMeasurements(painter, _client.m_worker->track.measurement_pack_list_);
      plotMeasurements(painter, track2.measurement_pack_list_);
     // plotMeasurements(painter, measurement_pack_list);

    // @brief 最终的航迹信息在此显示
   // plotTracks(painter, _client.m_worker->track.track_data_output_pack_list_);
    plotTracks(painter, track2.track_data_output_pack_list_);

    //@brief 可靠航迹信息在此显示
    //plotTrustTracks(painter, track2.trust_track_pack_list_);
    // @brief:  动态扫描的部分在此显示



    /*
    static double MyScanLine ;
    MyScanLine += static_cast<double>(2);
    if (MyScanLine >= 360.0) {
        MyScanLine = 0;
    }
    painter.translate(-kRoundDiameter/2,-kRoundDiameter/2);
    QConicalGradient gradient(kRoundDiameter/2,kRoundDiameter/2, MyScanLine);
    gradient.setColorAt(0.2,QColor(47,79,79, 100));
    gradient.setColorAt(0.5,QColor(47,79,79, 0));
    painter.setBrush(gradient);
    painter.setPen(QPen(Qt::NoPen));
    painter.drawPie(0, 0, kRoundDiameter,kRoundDiameter, MyScanLine * 16.0, 60* 16);
*/
    painter.end();
    pixmap.scaled(ui->label_main->size(), Qt::KeepAspectRatio);
    ui->label_main->setPixmap(pixmap);

}


/**
 * @brief MainWindow::~MainWindow
 * 说明：析构函数
 */
MainWindow::~MainWindow()
{
    delete ui;
}







