#pragma once


#include <vector>
#include <memory>
#include <map>
#include <cmath>
#include "base_objects.h"
#include <cstdint>
#include <QString>
/**
 * @brief The china struct
 */
struct China {
    China(Index i, PolyGon &&p) :
        index(i), polygon(std::move(p))
    {
    }

    ~China() {}

    Index index;
    QString id;
    PolyGon polygon;

};


struct AdjMap {

    std::vector<std::unique_ptr<China>> raw_china;
    std::map<NodeId, std::unique_ptr<China>> china_map;
    AdjMap() {}
    AdjMap(const AdjMap &) = delete;
    AdjMap &operator=(const AdjMap &) = delete;
};


