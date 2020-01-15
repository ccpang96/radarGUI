#pragma once

#include <string>
#include <functional>
#include <vector>
#include <map>

#include "base_objects.h"

class Dbf {
public:
    Dbf(const std::string &path);
    ~Dbf();

    std::string getStringField(Index index, const std::string &field_name);
    Row getRow(Index index);
    void designateValidFields(const std::vector<std::string> &field_names);

    Index numEntities() const
    {
        return this->_n_entities;
    }

private:
    enum class Type : int32_t {
        String,
        Integer,
        Double,
        Logical,
        Invalid
    };

    static const char *toString(const Type &type);

    //用于描述DBF中字段类型及名称的结构体
    struct Field {
        Type type;  //字段类型
        std::string name;  //字段名称

        Field() {}
        Field(Type type, std::string &&name);
        Field(const Field &);
        Field &operator=(const Field &); //重载赋值运算符
    };

    std::vector<Field> _fields;  //保存字段的类型及名称的数据结构
    std::vector<size_t> _designated_field_indices;  //保存_fields中的第几个字段被指定

    Index _n_entities;  //DBF的记录个数
    int _n_fields;      //DBF的字段个数
    void *_handle;
};
