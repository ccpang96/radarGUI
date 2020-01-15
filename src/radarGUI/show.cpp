#include "include/radarGUI/show.h"
#include "include/radarGUI/mainwindow.h"
#include <QDebug>


/**
 * @brief SetGUISize
 * 说明：获得屏幕的分辨率
 */
void SetGUISize() {

    /**
     * @brief 获取单个屏幕的分辨率
     */
    QDesktopWidget *desktop = QApplication::desktop();
    QRect screen = desktop->screenGeometry();
    int screenWidth = screen.width();  //屏幕的宽度
    int screenHeight = screen.height(); //屏幕的高度
    qDebug("width %d, height %d",  screenWidth, screenHeight);

    /**
     * @brief 获取多个屏幕的分辨率
     */
    /*
    int screenNum = desktop->screenCount();
    for (int i=0; i<screenNum; i++)
    {
        QRect screen = desktop->screenGeometry();
        qDebug("screen %d, width %d, height %d", i, screen.width(), screen.height());
    }
    */

    //
}


/**
 * @brief MainInterface
 * @param painter
 */
void MainInterface(QPainter &painter) {

    //将画移动到圆的中心
    painter.translate(QPoint((kRectangleWidth-kRoundDiameter)/2+kRoundDiameter/2,(kRectangleHigh - kRoundDiameter)/2+kRoundDiameter/2));

    QColor qcolor_round_background(0,0,0); //背景色
    painter.setRenderHint(QPainter::Antialiasing, true);  //反走样
    painter.setBrush(qcolor_round_background);  //设置画刷的颜色
    painter.setPen(Style_of_inner_circle_line); //设置圆内线的样式

    DrawCicle(painter,7);   //绘制7个圆
    DrawDiagonal(painter,30);   //每隔30度绘制一条斜线

    //设置刻度线的样式
    painter.setPen(color_of_scale);

    DrawScale(painter,6,2);  //绘制刻度

    //将画布移动到左上角
    painter.translate(QPoint(-(kRectangleWidth-kRoundDiameter)/2 - kRoundDiameter/2,-(kRectangleHigh - kRoundDiameter)/2-kRoundDiameter/2));

    DrawScaleValue(painter);  //绘制刻度值

    DrawDateAndOthers(painter);  //绘制时间及方位

    //将画布移动到圆的中心
    painter.translate(QPoint((kRectangleWidth-kRoundDiameter)/2+kRoundDiameter/2,(kRectangleHigh - kRoundDiameter)/2+kRoundDiameter/2));

}

//绘制圆
//circle_num：绘制circle_num个同心圆
void DrawCicle(QPainter &painter, const int &circle_num) {  //circle_num,

    int temp_x,temp_y;
    //用于修补最大圆不显示的问题
    temp_x = -kRoundDiameter/2-1;  //正切点
    temp_y = -kRoundDiameter/2-1; //正切点
    painter.drawEllipse(temp_x,temp_y,kRoundDiameter+2,kRoundDiameter+2);   //外层最大圆

    //绘制circle_num个圆
    for (int i = 0; i < circle_num; ++i) {
        temp_x = -(kRoundDiameter-i*(kRoundDiameter/circle_num))/2;
        temp_y = -(kRoundDiameter-i*(kRoundDiameter/circle_num))/2;
        painter.drawEllipse(temp_x,temp_y,kRoundDiameter - i*(kRoundDiameter/circle_num),kRoundDiameter - i*(kRoundDiameter/circle_num));
    }

    //用于画内层的最小圆
    temp_x = -kRoundDiameter/14;  //正切点
    temp_y = -kRoundDiameter/14; //正切点
    painter.drawEllipse(temp_x,temp_y,kRoundDiameter/7,kRoundDiameter/7);   //最小圆
}

//绘制斜线
//interval_angle:以interval_angle度为间隔绘制一条斜线
void DrawDiagonal(QPainter &painter,const int &interval_angle) {
    //interval_angle : 是每间隔多少度，画一条斜线
    for (int i = 0; i < 360 /interval_angle; ++i) {
        painter.rotate(interval_angle);
        painter.drawLine(0,0,kRoundDiameter/2,0);
    }
}

//绘制刻度：
//long_scale_length：以10度为间隔的刻度的长度
//short_scale_length:以1度为间隔的刻度的长度
void DrawScale(QPainter &painter,const int &long_scale_length,const int &short_scale_length) {
    for (unsigned char i=0; i<36; i++)
    {
        painter.drawLine(0, (kRoundDiameter/2) + long_scale_length, 0, kRoundDiameter/2);    //每隔10度绘制1次
        painter.rotate(10);
    }

    for (unsigned short i=0; i<360; i++)
    {
        painter.drawLine(0, (kRoundDiameter/2)+short_scale_length, 0, kRoundDiameter/2);    //每隔1度绘制1次
        painter.rotate(1);
    }
}


//绘制刻度值 ：这个绘制刻度值的函数是经过一次次试得到的，写的非常烂，所以写死了！！
void DrawScaleValue(QPainter &painter) {

    int center_circle_x = (kRectangleWidth - kRoundDiameter)/2 + kRoundDiameter/2;
    int center_circle_y = (kRectangleHigh - kRoundDiameter) /2 + kRoundDiameter/2;
    for(int i = 0; i < 36; ++i)
    {
        int scale_value = i *10;
        double sta = M_PI *scale_value / 180;
        double x = qSin(sta);
        double y = qCos(sta);
        double p = (scale_value > 90 ? (scale_value < 190 ? 1.06: (scale_value >= 310 ? (scale_value == 350 ? 1.05 : 1.07) : 1.1)) : 1.04);
        if (scale_value == 190)  p = 1.08;
        if (scale_value == 310 || scale_value == 320) p = 1.08;
        int temp_x = center_circle_x + (p * kRoundDiameter/2 ) * x ;
        int temp_y = center_circle_y - (p * kRoundDiameter/2 ) * y ;
        QFont font ( "Microsoft YaHei", 10, 100);
        painter.setFont(font);
        painter.drawText(temp_x,temp_y, QString::number(scale_value));

    }
}



void DrawDateAndOthers(QPainter &painter) {
    //获取日期和时间
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString current_date = current_date_time.toString("yyyy-MM-dd hh:mm:ss ddd");
    //显示标题
    painter.drawText(10,20,kMyTitle);
    //显示时间
    painter.drawText(10,40,current_date);
    //显示公司名称
    //painter.drawText(20,40,kCompanyName);
    //显示方位
    //painter.drawText(20,60,"方位:");
    //显示标题
}


//绘制十字架
//QLine qline1(-kRoundDiameter/2,0,kRoundDiameter/2,0);
//QLine qline2(0,kRoundDiameter/2,0,-kRoundDiameter/2);
//painter.drawLine(qline1);
//painter.drawLine(qline2);


















