#include "include/setParameter/dbf_to_dam.h"

/**
 * @brief DBFTODAM::DBFTODAM
 * @说明：默认构造函数
 */
DBFTODAM::DBFTODAM() {


    //协议包头，使用如下定义值
    //WORD 1
     this->head_key = 0x7FFFBC1C;
    //WORD 2
     this->head_2 = 0xAAAABBBB;
    //WORD 3
     this->head_3 = 0x3C1C7FFF;
    //WORD 4
     this->head_4 = 0x7FFF7FFF;
    //WORD 5
     this->head_5 = 0x12345678;


    /**
      此处是你要传输的数据
    **/


    //协议包尾
    this->tail01 = 0x87654321;		//0x87654321
    this->tail02 = 0x7FFFBC3C ;		//0x7FFFBC3C

}
