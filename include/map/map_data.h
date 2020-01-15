#pragma once

#include "include/map/china_loader.h"
#include <vector>
#include <QPainter>

struct MapData {

    ChinaLoader china_loader;
    Box box;

    MapData();
    void getChinaMap(); //将shp文件存储到vector中
    void drawChina(QPainter *painter);
    Point drawChina(Point &radar_location, quint16 &radar_detection_distance);
    double getDistancefromll(Point &p1, Point &p2);
    double getDistance(Point &p1, Point &p2);
    PolyLine drawChinafromll( Point &radar_location,  double &radar_detection_distance);
    void load();
    std::vector<PolyLine> china_map; //存储中国地图

private:
    double  PI = 3.14159265358979;
};
