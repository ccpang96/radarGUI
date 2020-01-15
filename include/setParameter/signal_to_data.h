#ifndef SIGNAL_TO_DATA
#define SIGNAL_TO_DATA


/*
struct TotalInfo{
  char head;
  struct PointInfo Point[100];
  long lTimeStampInfo; //时间是us
  char tail;
};
*/

struct PointInfo{
  double speed;
  double distance;
  double angle;
};


struct SIGNALTODATA {

    //协议包头，使用如下字符
    char head = '{';
    int package_valid_number;
    PointInfo Point[500];
    long lTimeStampInfo;  //单位是us
    char tail = '}';

};
#endif // SIGNAL_TO_DATA

