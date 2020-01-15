#include "include/map/map_data.h"
#include <QDebug>
#include <vector>
#include "include/radarGUI/mainwindow.h"
MapData::MapData()
    : china_loader(MOCT_CHINA_PATH)
{

}

/**
 * @brief MapData::load
 * @说明：获取map的边界值
 */
void MapData::load()
{
    const Box &china_box = china_loader.getBox();
    double minx = china_box.x_min;
    double miny = china_box.y_min;
    double maxx = china_box.x_max;
    double maxy = china_box.y_max;
    std::cout << "left bottom point: (" << minx << ", " << miny << ")" << std::endl;
    std::cout << "right top point: (" << maxx << ", " << maxy << ")" << std::endl;

    box.x_max = maxx;
    box.x_min = minx;
    box.y_max = maxy;
    box.y_min = miny;
}


/**
 * @brief MapData::getChinaMap
 * @说明： 将shp文件保存到vector中
 */
void MapData::getChinaMap() {

    Index all_num = china_loader.numEntities();
    PolyLine china_polyline;  //每个parts
    for(Index i = 0; i < all_num; ++i) {
      china_polyline = china_loader._shp.getPolyline(i);
      china_map.push_back(china_polyline);
    }
}


/**
 * @brief MapData::drawChina
 * @param painter
 * @ 地图在此绘制
 */
void MapData::drawChina(QPainter *painter)
{
    Q_UNUSED(painter);
    Index all_num = china_loader.numEntities();
    std::cout << all_num << std::endl;

    PolyGon china_polygon;  //每个parts

    for(Index i = 0; i < all_num; ++i) {
      china_polygon = china_loader._shp.getPolygon(i);
    }

    if (china_polygon.parts.empty())
        throw std::invalid_argument("link.polyline.parts is empty!");

    for (auto &part : china_polygon.parts) {
        std::vector<QPointF> points;
        if (part.size() < 2)
            throw std::invalid_argument("invalid part.. points is smaller than two");

        QPointF p1(part[0].x, part[0].y);
        for (auto it = part.begin() + 1; it != part.end(); ++it) {
            QPointF p2(it->x, it->y);
            //painter.drawLine(p1, p2);
            p1 = std::move(p2);
        }
    }

}


/**
 * @brief MapData::getDistance
 * @param p1
 * @param p2
 * @获取两点之间的距离
 */
double MapData::getDistance(Point &p1, Point &p2) {
    double dist;
    double x = double(p1.x - p2.x);
    double y = double(p1.y - p2.y);
    dist = sqrt(x*x + y*y);
    return dist;

}
/**
 * @brief MapData::drawChina
 * @param radar_location 探测雷达所在的经纬度
 * @param radar_detection_distance 探测雷达所能探测到的最大探测距离
 * @说明
 */
Point MapData::drawChina(Point &radar_location, quint16 &radar_detection_distance)
{

    Q_UNUSED(radar_detection_distance);
    double short_distance = 1000000; //这里给一个极大值
    size_t short_i = 0,short_j = 0,short_k = 0;
    if (china_map.empty())
        throw std::invalid_argument("link.polyline.parts is empty!");

    qDebug() << " china_map.size" << china_map.size() << endl; //

    for (size_t i = 0; i < china_map.size(); ++i) {  //
        for (size_t j = 0; j < china_map[i].parts.size(); ++j) {

            double dist;
            dist = getDistance(radar_location,(china_map[i].parts[j])[0]);
            if (dist < short_distance) {
                short_distance = dist;
                short_i = i ;
                short_j = j;
            }
            qDebug() << "i:" <<i << "j:" << j<< "short_distance:" << short_distance<<endl;
            qDebug() << (china_map[i].parts[j])[0].x << "  "<<  (china_map[i].parts[j])[0].y << endl; //和这个值比较
            //qDebug() << "china_map_parts size :" << china_map[i].parts[j].size() << endl;
        }
    }

    for(size_t k = 0; k < china_map[short_i].parts[short_j].size(); ++k) {
        double dist;
        dist = getDistance(radar_location,(china_map[short_i].parts[short_j])[k]);
        if (dist < short_distance) {
            short_distance = dist;
            short_k = k;
        }
        qDebug() << "i:" << short_i << "j:" << short_j<< "short_k: " << short_k << "short_distance:" << short_distance<<endl;
        qDebug() << (china_map[short_i].parts[short_j])[k].x << "  "<<  (china_map[short_i].parts[short_j])[k].y << endl;
    }

    Point short_longitude_latitude{(china_map[short_i].parts[short_j])[short_k].x, (china_map[short_i].parts[short_j])[short_k].y};
    return short_longitude_latitude;

}


/**
 * @brief MapData::getDistance
 * @param p1
 * @param p2
 * @说明： 根据两个点的经纬度计算距离
 */
double MapData::getDistancefromll(Point &p1, Point &p2) {
    double dist;

    double R =6378.1370; //地球半径 6371km

    double x1,x2,y1,y2;

    //qDebug() << p1.y << p1.x<<endl;
    y1 = p1.y*PI /180.0;
    x1 = p1.x *PI/180.0;

    y2 = p2.y*PI /180.0;
    x2 = p2.x *PI/180.0;

   // qDebug() << p1.y << p1.x<<endl;

    double cc1 = sin(y1) * sin(y2) + cos(y1) * cos(y2)*cos(x1-x2);
    dist = acos(cc1) *R;
    //qDebug() << "dist: " << dist << endl;
    return dist;

}



/**
 * @brief MapData::drawChina
 * @param radar_location 探测雷达所在的经纬度
 * @param radar_detection_distance 探测雷达所能探测到的最大探测距离
 * @说明
 */
 PolyLine MapData::drawChinafromll(Point &radar_location,  double &radar_detection_distance)
{
    PolyLine radar_map;
    size_t short_i = 0,short_j = 0,short_k = 0;
    if (china_map.empty())
        throw std::invalid_argument("link.polyline.parts is empty!");
    qDebug() << " china_map.size" << china_map.size() << endl; //

    for (size_t i = 0; i < china_map.size(); ++i) {  //

        for (size_t j = 0; j < china_map[i].parts.size(); ++j) {

            double dist;
            dist = getDistancefromll(radar_location,(china_map[i].parts[j])[0]);
            if (dist < radar_detection_distance) { //当part0在范围之内
                short_i = i ;
                short_j = j;

                std::vector<Point> part;
                for(size_t k = 0; k < china_map[short_i].parts[short_j].size(); ++k) {
                    double dist1;
                    dist1 = getDistancefromll(radar_location,(china_map[short_i].parts[short_j])[k]);
                    if (dist1 < radar_detection_distance) {
                        short_k = k;
                        Point short_longitude_latitude{  111.7 * kRangeRadarMap/2.0 /radar_detection_distance*cos(radar_location.y * PI /180.0) *((china_map[short_i].parts[short_j])[short_k].x - radar_location.x), \
                                  kRangeRadarMap/2.0 /radar_detection_distance * 111.7*((china_map[short_i].parts[short_j])[short_k].y - radar_location.y)};
                        //qDebug() << short_longitude_latitude.x << short_longitude_latitude.y <<endl; //打印

                        part.emplace_back(short_longitude_latitude);
                    }
                }
                radar_map.parts.emplace_back(part);
            }
        }

    }
    qDebug() << "map_data  " << radar_map.parts.size() << endl;
    return radar_map;
}































