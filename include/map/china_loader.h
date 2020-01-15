#ifndef CHINA_MAP_H
#define CHINA_MAP_H

#include <cmath>
#include <string>
#include <iostream>

#include "loader.h"


#define MOCT_CHINA_PATH  "E:\\radarGUI\\data\\china_basic_map\\bou2_4l"

class ChinaLoader: public Loader {
public:
    ChinaLoader(const std::string &path) : Loader(path) {}  //构造函数

    virtual void load(AdjMap &adj_map) override;
    //必须重载基类的同名函数

    PolyGon getPolygon(Index index)
    {
        return this->_shp.getPolygon(index);
    }

    Row getRow(Index index)
    {
        return this->_dbf.getRow(index);
    }
};

#endif // CHINA_MAP_H
