#ifndef INCLUDE_RADARGUI_SHOW_H
#define INCLUDE_RADARGUI_SHOW_H

#include <QtMath>
#include <QPixmap>
#include <QPainter>
#include <QApplication>
#include "QRect"    //下面这个两个头文件是为了获得屏幕的分辨率
#include <QDesktopWidget>

//字体大小
const int point_size = 1;
const QString kMyTitle{"radarGUI"};
const QString kCompanyName{"UESTC"};
//设置圆内线的样式
//static QColor color_of_inner_circle_line(46,139,87);
static QColor color_of_inner_circle_line(0,128,0); //3,35,14
static QPen Style_of_inner_circle_line = QPen(color_of_inner_circle_line, 1, Qt::DashDotLine, Qt::RoundCap);

//设置刻度线的样式
//static QColor color_of_scale(70,130,180);  //设置绘制刻度的笔的颜色及大小等
static QColor color_of_scale(255,140,0);
static QPen Style_of_scale = QPen(color_of_scale, 2, Qt::DashDotLine, Qt::RoundCap);
//设置tab_GUI的背景色

//设置tab_SET的背景色





void MainInterface(QPainter &painter);
void DrawCicle(QPainter &painter,const int &circle_num);  //绘制circle_num个圆
void DrawDiagonal(QPainter &painter,const int &interval_angle);  //每间隔interval_angle度,绘制一条斜线
void DrawScale(QPainter &painter,const int &long_scale_length,const int &short_scale_length);  //以10度为间隔，绘制长度为long_scale_length的长刻度，以1位间隔
//绘制长度为short_scale_length的短刻度
void DrawScaleValue(QPainter &painter);
void DrawDateAndOthers(QPainter &painter);
void SetGUISize();

#endif // INCLUDE_RADARGUI_SHOW_H
