#ifndef TRACK_PACKAGE_H
#define TRACK_PACKAGE_H


#include "Eigen/Dense"

struct MeasurementPackage {
    /**
     * @brief The MeasurementPackage class
     * raw_measurements_:
     * 0.径向距离
     * 1.方位角
     * 2.径向速度
     */
  Eigen::VectorXd raw_measurements_;
};

struct NowFramePoint {

    /**
     * @brief raw_now_frame_point_
     * 说明：每次从CFAR处理后回来的点迹
     * point_ : 距离、速度、方位
     * time_accumulate: 积累时间，即从第一帧（批）数据到当前帧（批）数据之间一共经过的时间
     * raw_now_frame_point_ : 当前点迹的属性 一共4个值
     * 0.通道号
     * 1.点迹未用的次数
     * 2.点迹消亡的门限次数 :5
     * 3.bool值：表明已用/未用 1已用 0未用
     */
    Eigen::Vector3d now_point_;   //当前点迹数据
    double time_accumulate_;
    Eigen::Vector4i raw_now_frame_point_;

};

/**
 * @brief The TempPoint struct
 * 说明：临时点迹数据
 * 存储航迹起始和点迹航迹关联时没有用到的点迹
 */
struct TempPointPackage {

    /**
     * @brief temp_point_
     * 说明：
     * temp_point_ : 距离、角度、速度
     * time_accumulate: 积累时间 （有可能是小数呢？）
     * raw_temp_point_ : 临时点迹的属性  一共4个值
     * 0.通道号
     * 1.点迹未用的次数
     * 2.点迹消除的门限次数：5
     * 3.bool值：表明已用/未用 1已用 0 未用
     */
    Eigen::Vector3d temp_point_;  //临时点迹数据
    double time_accumulate_;
    Eigen::Vector4i raw_temp_point_;

};


/**
 * @brief The TrustTrack struct
 * 说明：可靠航迹数据
 * 存储已经形成的可靠航迹的信息，保存的每条可靠航迹的最后一个点的信息
 */
struct TrustTrackPackage {

    /**
     * @brief trust_track_
     * 说明：
     * last_point_x_中存储x,y,vx,vy
     * last_point_P_ : 最后一个点的协方差矩阵
     * raw_track_data_output_:可靠航迹属性
     * 0.航迹识别标志
     * 1.来/去
     * 2.是第几个点
     * 3.实点/补点  实点为0 补点为1
     * 4.属于哪个通道
     * raw_trust_track_ :可靠航迹的属性  一共3个值
     * 0.航迹未被实测数据更新次数
     * 1.航迹更新标志 bool值 0 未更新 1 更新
     * 2.航迹消亡的未被更新次数门限值
     */
    Eigen::Vector4d last_point_x_;  //可靠航迹最后一个点的状态变量信息
    Eigen::Matrix4d last_point_P_ = Eigen::MatrixXd(4, 4);    //可靠航迹中最后一个点的状态协方差矩阵P
    Eigen::VectorXi raw_track_data_output_;
    Eigen::Vector3i raw_trust_track_;

};


/**
 * @brief The TrackDataOutput struct
 * 说明：本时刻批次数据处理完毕后，输出的航迹信息，保存的可靠航迹的所有点信息，画图的时候就画这个数据包
 * track_dataout_point_ : 距离、角度、速度
 * time_accumulate: 积累时间
 * raw_track_data_output_: 一共5个值
 * 0.航迹识别标志
 * 1.来/去
 * 2.是第几个点
 * 3.实点/补点 实点为0 补点为1
 * 4.属于第几个通道
 */
struct TrackDataOutputPackage {


    Eigen::Vector3d track_dataout_point_;
    double time_accumulate_;
    Eigen::VectorXi raw_track_data_output_;

};


#endif // TRACK_PACKAGE_H
































