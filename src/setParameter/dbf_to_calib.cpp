#include "include/setParameter/dbf_to_calib.h"
DBFTOCALIB::DBFTOCALIB () {

    //协议包头，使用如下定义值
    //WORD 1
    /* unsigned int */ this->head_key = 0x7FFFBC1C ; //0x7FFFBC1C
    //WORD 2
    /* unsigned int */ this->head_2 =  0xAAAABBBB ; //0xAAAABBBB
    //WORD 3
    /* unsigned int */ this->head_3 = 0x3C1C7FFF ; //0x3C1C7FFFF
    //WORD 4
    /* unsigned int */ this->head_4 = 0x7FFF7FFF ; //0x7FFF7FFF
    //WORD 5
    /* unsigned int */ this->head_5 = 0x12345678 ; //0x12345678


    //协议包尾
    //WORD 63
    /* unsigned int */ this->tail01 = 0x87654321;		//0x87654321
    //WORD 64
    /* unsigned int */ this->tail02 = 0x7FFFBC3C;		//0x7FFFBC3C
}
