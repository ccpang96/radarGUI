#ifndef TRACK_START_H
#define TRACK_START_H
#include <vector>
#include "include/radarGUI/track_package.h"
#include "include/radarGUI/kalman_filter.h"
#include "include/radarGUI/FusionEKF.h"
using namespace std;


class Track {
public:

    Track();

    void  track_object();   //航迹管理函数：
    void track_start(double K_start);   //航迹起始函数
    void point_track_association(double K_association); //航迹关联函数
    void point_supplement();  //航迹补点函数
    void track_die_out();   //航迹消亡函数

     vector<MeasurementPackage> measurement_pack_list_ ; //经过CFAR后的数据包
     vector<TrackDataOutputPackage> track_data_output_pack_list_;  //本批数据处理完成后，输出的航迹信息 存放所有的航迹信息
     vector<NowFramePoint> now_frame_point_pack_list_;  //当前帧的点数据包
     vector<TempPointPackage> temp_point_pack_list_;    //临时点迹数据  没有关联到航迹中的孤点
     vector<TrustTrackPackage> trust_track_pack_list_; //可靠航迹数据 的最后一个点的信息
     double  time_accumulate_ = 0.0; //积累时间
     int number_of_track_ = 0; //航迹总数
     int number_frame_ = 0;
     double last_time_ = 0.0; //上次数据的时间
     double dt_ = 0.0;  //两批数据之间的间隔
private:
      Tools tools2;
};


#endif // TRACK_START_H
