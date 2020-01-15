#include "include/map/loader.h"
#include  <QDebug>

/**
 * @brief Loader::Loader
 * @param path
 */
Loader::Loader(const std::string &path) : _dbf(path), _shp(path)
{
    if (this->_dbf.numEntities() != this->_shp.numEntities())
        throw std::runtime_error("dbf's entity and shp's entity number is different!");
    this->_n_entities = this->_dbf.numEntities();
}



