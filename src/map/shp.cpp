#include "shape/shapefil.h"
#include <string>
#include <sstream>
#include <memory>
#include <stdexcept>
#include <algorithm>
#include <iostream>
#include "include/map/shp.h"

/**
 * 匿名空间：internal 链接属性
 */
namespace {

inline Point toPoint(SHPObject *object)
{
    Point point;
    point.x = object->padfX[0];
    point.y = object->padfY[0];
    return point;
}

inline PolyLine toPolyline(SHPObject *object)
{
    PolyLine polyline;

    /*
    polyline.box.x_max = object->dfXMax;
    polyline.box.x_min = object->dfXMin;
    polyline.box.y_max = object->dfYMax;
    polyline.box.y_min = object->dfYMin;
    */

    if (object->nParts == 0 || object->nParts == 1) {
        std::vector<Point> part;
        for (int i = 0; i < object->nVertices; ++i) {
            Point point;
            point.x = object->padfX[i];
            point.y = object->padfY[i];
            //emplace_back()会在容器管理的内存中直接创建对象
            //所以此处改成part.emplace_back(point.x,point.y);
            //将参数完美转发给Point的构造函数，在vector容器中直接构造一个元素，为了节省一点执行时间，C++也是拼了。
            //std::move(point)：将一个左值引用强制转换为一个右值引用
            part.emplace_back(std::move(point));
        }
       // std::cout << "part's size: " << part.size() <<std::endl;
        polyline.parts.emplace_back(std::move(part));
    } else {
        //nVertices是所有part的总长度，panPartStart是代表每个part的起始点
        int part_index = 1;
        int next_part_start_index = object->panPartStart[part_index]; //第i个part的起始位置
        std::vector<Point> part;
        for (int i = 0; i < object->nVertices; ++i) {
            Point point;
            point.x = object->padfX[i];
            point.y = object->padfY[i];
            part.emplace_back(std::move(point));

            if (i + 1 == next_part_start_index) {
                polyline.parts.emplace_back(std::move(part));
                part.clear();
                if (part_index + 1 == object->nParts) //到达最后一个part
                    next_part_start_index = object->nVertices;
                else
                    next_part_start_index = object->panPartStart[++part_index];
            }
        }
    }

    return polyline;
}


inline PolyGon toPolygon(SHPObject *object)
{
    PolyGon polygon;

    /*
    polygon.box.x_max = object->dfXMax;
    polygon.box.x_min = object->dfXMin;
    polygon.box.y_max = object->dfYMax;
    polygon.box.y_min = object->dfYMin;
    */

    if (object->nParts == 1) { //只有一块
        std::vector<Point> part;
        for (int i = 0; i < object->nVertices; ++i) {
            Point point;
            point.x = object->padfX[i];
            point.y = object->padfY[i];
            //emplace_back()会在容器管理的内存中直接创建对象
            //所以此处改成part.emplace_back(point.x,point.y);
            //将参数完美转发给Point的构造函数，在vector容器中直接构造一个元素，为了节省一点执行时间，C++也是拼了。
            //std::move(point)：将一个左值引用强制转换为一个右值引用
            part.emplace_back(std::move(point));
        }
           //std::cout << "nparts = 0 or nparts = 1" << std::endl;
        //std::cout << "part's size: " << part.size() <<std::endl;
        polygon.parts.emplace_back(std::move(part));
    } else {
        //nVertices是所有part的总长度，panPartStart是代表每个part的起始点
        int part_index = 1;
        int next_part_start_index = object->panPartStart[part_index]; //第i个part的起始位置
        std::vector<Point> part;
        for (int i = 0; i < object->nVertices; ++i) {
            Point point;
            point.x = object->padfX[i];
            point.y = object->padfY[i];
            part.emplace_back(std::move(point));

            if (i + 1 == next_part_start_index) {
                polygon.parts.emplace_back(std::move(part));
                part.clear();
                if (part_index + 1 == object->nParts) //到达最后一个part
                    next_part_start_index = object->nVertices;
                else
                    next_part_start_index = object->panPartStart[++part_index];
            }
        }
        //std::cout << "nparts > 1" << std::endl;
    }

    return polygon;
}
} // namespace anonymous



/**
 * @brief Shp::Shp
 * @param path:存放shp文件的路径，使用相对路径 ".//data//City//CN_city"
 */
Shp::Shp(const std::string &path) :
    pn_entities_(0),
    pn_shape_type_(ShapeType::Invalid),
    handle_(static_cast<void *>(SHPOpen(path.c_str(), "rb")))
{
    if (this->handle_ == nullptr)
        throw std::runtime_error("Failed to SHPOpen...");  //运行时错误

    int shape_type = static_cast<int>(ShapeType::Invalid);  //无效类型
    double min_bound[4] = { 0, };
    double max_bound[4] = { 0, };
    /**
     * @brief SHPGetInfo
     * 获取shapefile的全局信息
     */
    SHPGetInfo(
        static_cast<SHPHandle>(this->handle_),
        reinterpret_cast<int *>(&this->pn_entities_),
        &shape_type, min_bound, max_bound);

    this->pn_shape_type_ = static_cast<ShapeType>(shape_type);  //是哪种类型

    //形状个数
    if (this->pn_entities_ <= 0) {
        std::cerr << "_n_entities is invalid: " << this->pn_entities_ << std::endl;
        throw std::runtime_error("SHPGetInfo returns invalid entity num...");
    }

    /**
     * map的边界
     */

    this->_box.x_min = min_bound[0];
    this->_box.x_max = max_bound[0];
    this->_box.y_min = min_bound[1];
    this->_box.y_max = max_bound[1];

    std::cout << "box xmin(" << this->_box.x_min << ") xmax(" << this->_box.x_max
        << ") ymin(" << this->_box.y_min << ") ymax(" << this->_box.y_max << ")" << std::endl;

    std::cout << "[" << path << "] shape type[" << static_cast<int>(this->pn_shape_type_)
        << "] entity num[" << this->pn_entities_ << "]" << std::endl;
}


/**
 * @brief Shp::~Shp
 * 析构函数：关闭打开的文件
 */
Shp::~Shp()
{
    SHPClose(static_cast<SHPHandle>(this->handle_));
}



/**
 * @brief Shp::getPolygon
 * @param index
 * @return
 * @说明：获得圆弧形状的经纬度
 */
PolyGon Shp::getPolygon(Index index)
{
    if (this->pn_shape_type_ != ShapeType::Polygon) //多边形
        throw std::runtime_error("this shape type isn't polygon!");

    std::unique_ptr<SHPObject, void(*)(SHPObject *)> object(
        SHPReadObject(
            static_cast<SHPHandle>(this->handle_),
            static_cast<int>(index)),
        SHPDestroyObject);

    if (object == nullptr)
        throw std::invalid_argument("index may be out of range");

    return toPolygon(object.get()); //返回object中保存的指针，
}


/**
 * @brief Shp::getPolyline
 * @param index
 * @return
 * @说明：获得圆弧形状的经纬度
 */
PolyLine Shp::getPolyline(Index index)
{
    if (this->pn_shape_type_ != ShapeType::Polyline) //圆弧形
        throw std::runtime_error("this shape type isn't polyline!");

    std::unique_ptr<SHPObject, void(*)(SHPObject *)> object(
        SHPReadObject(
            static_cast<SHPHandle>(this->handle_),
            static_cast<int>(index)), //第几个形状
        SHPDestroyObject);

    if (object == nullptr)
        throw std::invalid_argument("index may be out of range");

    return toPolyline(object.get()); //返回object中保存的指针，
}


/**
 * @brief Shp::getPoint
 * @param index
 * @return
 * @说明：得到形状为point点的所有经纬度
 * 获取第index形状的经纬度，
 */
Point Shp::getPoint(Index index)
{
    if (this->pn_shape_type_ != ShapeType::Point)
        throw std::runtime_error("this shape type isn't point!");

    std::unique_ptr<SHPObject, void(*)(SHPObject *)> object(
        SHPReadObject(
            static_cast<SHPHandle>(this->handle_),
            static_cast<int>(index)),
        SHPDestroyObject);

    if (object == nullptr)
        throw std::invalid_argument("index may be out of range");

    return toPoint(object.get());
}


