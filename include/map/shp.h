#ifndef SHP_H
#define SHP_H
#include <string>
#include <functional>
#include <vector>
#include "base_objects.h"

class Shp {
public:
    Shp(const std::string &path);
    ~Shp();

    PolyLine getPolyline(Index index);
    PolyGon getPolygon(Index index);        //获取多边形的信息
    Point getPoint(Index index);

    Index numEntities() const
    {
        return this->pn_entities_;
    }

    ShapeType getShapeType() const
    {
        return this->pn_shape_type_;
    }

    const Box &getBox() const
    {
        return this->_box;
    }

private:
    Index pn_entities_;  //number of structure uint32_t
    ShapeType pn_shape_type_; //shape的类型
    void * handle_;
    Box _box;
};


#endif // SHP_H
