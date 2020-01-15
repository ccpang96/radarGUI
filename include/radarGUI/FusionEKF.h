#ifndef FusionEKF_H_
#define FusionEKF_H_

#include "Eigen/Dense"        //Eigen的矩阵库
#include <vector>
#include <string>
#include <fstream>
#include "kalman_filter.h"
#include "tools.h"
#include "track_package.h"

class FusionEKF {
public:
  /**
  * 构造函数
  */
  FusionEKF();

  /**
  * 析构函数
  */
  virtual ~FusionEKF();

  void onlyPredict(const Eigen::Vector4d &last_point_x_,  const Eigen::Matrix4d &last_point_P_, const double &dt);  //只做预测
  /**
  * 整个扩展卡尔曼滤波的主要flow
  */
void ProcessMeasurement(const Eigen::Vector3d &point_now);

  /**
  * 卡尔曼滤波的更新和预测数学部分
  */
  KalmanFilter ekf_;

  double noise_ax;
  double noise_ay;

private:
  //检查跟踪是否初始化，是否是第一部分
  bool is_initialized_;

  // 前一个时刻值
  long long previous_timestamp_;

  //用于计算Jacobian的工具
  Tools tools;
  Eigen::MatrixXd R_radar_;           // radar measurement noise
  Eigen::MatrixXd H_jacobian;         // measurement function for radar

};

#endif /* FusionEKF_H_ */
