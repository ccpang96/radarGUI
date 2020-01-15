#ifndef DBF_TO_CALIB_H
#define DBF_TO_CALIB_H
//根据王博士提供的最新协议文档整理
//日期：2019-11-19
//创建：Z.-X. Chen, Zhiming Huang

struct DBFTOCALIB
{

    DBFTOCALIB ();
    //协议包头，使用如下定义值
    //WORD 1
    unsigned int head_key;//0x7FFFBC1C
    //WORD 2
    unsigned int head_2;//0xAAAABBBB
    //WORD 3
    unsigned int head_3;//0x3C1C7FFFF
    //WORD 4
    unsigned int head_4;//0x7FFF7FFF
    //WORD 5
    unsigned int head_5;//0x12345678

    //协议包尾
    //WORD 63
    unsigned int tail01;		//0x87654321
    //WORD 64
    unsigned int tail02;		//0x7FFFBC3C

};
#endif // DBF_TO_CALIB_H
