#include <utility>
#include <vector>
#include <iostream>
#include <algorithm>
#include "shape/shapefil.h"
#include "dbf.h"
#include <QDebug>

/**
 * @brief Dbf::Dbf
 * @param path
 * @说明：DBF构造函数
 * @读取DBF字段长度、记录长度，并将字段类型以及字段名称存入_fields
 */
Dbf::Dbf(const std::string &path) :
    _handle(static_cast<void *>(DBFOpen(path.c_str(), "rb")))
{
    if (this->_handle == nullptr)
        throw std::runtime_error("Failed to DBFOpen...");

    this->_n_fields = DBFGetFieldCount(static_cast<DBFHandle>(this->_handle));  //有效字段个数
    this->_n_entities = DBFGetRecordCount(static_cast<DBFHandle>(this->_handle)); //记录个数


    std::cout << "[" << path << "]" << std::endl;
    std::cout << "dbf field number"<< this->_n_fields<< std::endl;
    std::cout << "dbf entities number " << this->_n_entities<<std::endl;



    //将DBF数据的字段类型及字段名称写进_fields
    for (int i = 0; i < this->_n_fields; ++i) {

        std::vector<char> field_name_buf(12, '\0');
        DBFFieldType dbftype = DBFGetFieldInfo(
            static_cast<DBFHandle>(this->_handle),
            i,  //第i个字段
            field_name_buf.data(), nullptr, nullptr);

        Dbf::Type type;
        switch (dbftype) {
        case FTString:
            type = Type::String;   //固定长度的字符串字段
            break;
        case FTInteger:
            type = Type::Integer;  //不带小数的数字字段
            break;
        case FTDouble:
            type = Type::Double;  //带小数的数字字段
            break;
        case FTLogical:
            type = Type::Logical;  //逻辑字段
            break;
        case FTInvalid:
        default:
            type = Type::Invalid;  //无法识别的字段类型
            break;
        }

       // std::cout << type << std::endl; 没有重载输出运算符
        this->_fields.emplace_back(
            type, std::string(field_name_buf.data()));
    }
}


/**
 * @brief Dbf::~Dbf
 * @name:DBF析构函数
 * @说明：关闭DBF文件
 */
Dbf::~Dbf()
{
    DBFClose(static_cast<DBFHandle>(this->_handle));
}


/**
 * @brief Dbf::designateValidFields
 * @param field_names
 * @功能：判断所给的字段名称field_name是否存在于DBF中
 * @_designated_field_indices用于保存_fields中的第几个字段被指定
 */
void Dbf::designateValidFields(const std::vector<std::string> &field_names)
{
    for (size_t i = 0; i < this->_fields.size(); ++i) {
        for (const auto &field_name : field_names) {
            if (field_name == this->_fields[i].name) {

                //_fields中的第i个字段被指定
                std::cout << "field designated: " << field_name << std::endl;
                this->_designated_field_indices.push_back(i);
                break;
            }
        }
    }
}

/**
 * @brief Dbf::getStringField
 * @param index
 * @param field_name 字段名称
 * @return DBF中field_name字段中第index值
 */
std::string Dbf::getStringField(Index index, const std::string &field_name)
{
    int field_index = DBFGetFieldIndex(
        static_cast<DBFHandle>(this->_handle), field_name.c_str()); //根据字段名称，返回字段编号
    if (field_index == -1)
        throw std::invalid_argument("invalid arg in Dbf::getStringField");

    const char *field_value = DBFReadStringAttribute(
        static_cast<DBFHandle>(this->_handle), index, field_index);//文件句柄、该字段的第几个值，第几个字段
    if (field_value == nullptr || field_value[0] == '\0') //field_value无效
        return std::string();
    else
        return std::string(field_value);
}

/**
 * @brief Dbf::getRow
 * @param index 要查询的是第几条记录
 * @return
 */
Row Dbf::getRow(Index index)
{
    Row row;
    for (auto i : this->_designated_field_indices) {
        switch (this->_fields[i].type) {

        case Type::Double:  //查询第i个字段的第index值，将其作为double返回
            row.doubles[this->_fields[i].name] =
                DBFReadDoubleAttribute(
                    static_cast<DBFHandle>(this->_handle),
                    static_cast<int>(index), static_cast<int>(i));
            break;

        case Type::Integer:  //查询第i个字段的第index值，将其作为整数返回
            row.integers[this->_fields[i].name] =
                DBFReadIntegerAttribute(
                    static_cast<DBFHandle>(this->_handle),
                    static_cast<int>(index), static_cast<int>(i));
            break;

        case Type::String: //查询第i个字段的第index值，将其作为string返回
        {
            const char *val = DBFReadStringAttribute(
                static_cast<DBFHandle>(this->_handle),
                static_cast<int>(index), static_cast<int>(i));
            row.strings[this->_fields[i].name] = (val == nullptr) ?
                std::string() : std::string(val);
            break;
        }

        default:
            throw std::invalid_argument("Unknown field type in dbf...");
        }
    }
    return row;  //存放的是index/double/string型字段所对应的值
}



Dbf::Field::Field(Dbf::Type typeIn, std::string &&nameIn) :
    type(typeIn), name(std::move(nameIn))
{
}

Dbf::Field::Field(const Dbf::Field &other) : type(other.type), name(other.name) {}

/**
 * @brief Dbf::Field::operator =
 * @param other
 * @说明：重载Field的赋值运算符
 */
Dbf::Field &Dbf::Field::operator=(const Dbf::Field &other)
{
    if (this == &other)
        return *this;

    this->type = other.type;
    this->name = other.name;

    return *this;
}

/**
 * @brief Dbf::toString
 * @param type
 * @return 将字段类型转换为string格式
 */
const char *Dbf::toString(const Type &type)
{
    switch (type) {
    case Type::String: return "String";
    case Type::Integer: return "Integer";
    case Type::Double: return "Double";
    case Type::Logical: return "Logical";
    case Type::Invalid:
    default:
        return "Invalid";
    }
}
