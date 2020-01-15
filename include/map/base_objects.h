#ifndef BASE_OBJECTS_H
#define BASE_OBJECTS_H

#include <vector>
#include <map>
#include <string>
#include <list>

using Index = uint32_t;
using NodeId = uint32_t;
using LinkId = uint32_t;



//地图的边界
struct Box {
    double x_min;
    double x_max;
    double y_min;
    double y_max;
};


//用于描述dbf中第index条记录所对应的字段的值
struct Row {
    std::map<std::string, std::string> strings;  //固定长度的字符串字段
    std::map<std::string, int> integers;    //不带小数的数字字段
    std::map<std::string, double> doubles;  //带小数的数字字段
};

// SHP shape types
enum class ShapeType : int {
    Invalid =  0x00,    //NULL shapes
    Point =    0x01,    //Points
    Polyline = 0x03,    //Arcs(圆弧形)
    Polygon =  0x05     //Ploygons(多边形)
};

// SHP type 点
struct Point {
    double x; //经度
    double y; //纬度
};

// SHP type 圆弧
struct PolyLine {
    //Box box;
    std::vector<std::vector<Point>> parts;
};

//SHP type 多边形
struct PolyGon {
   // Box box; //边界值
    std::vector<std::vector<Point>> parts; //每个parts
};

#endif // BASE_OBJECTS_H
