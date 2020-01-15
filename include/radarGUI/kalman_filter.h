#ifndef KALMAN_FILTER_H_
#define KALMAN_FILTER_H_
#include "Eigen/Dense"
#include "include/radarGUI/tools.h"

static Tools tools1;

class KalmanFilter {
public:

  // 状态变量x
  Eigen::VectorXd x_;

  //状态协方差矩阵P
  Eigen::MatrixXd P_;

  // 状态转移矩阵F
  Eigen::MatrixXd F_;

  // 处理过程协方差矩阵Q
  Eigen::MatrixXd Q_;

  // measurement matrix
  Eigen::MatrixXd H_;

  //测量协方差矩阵
  Eigen::MatrixXd R_;

  /**
   * 构造函数
   */
  KalmanFilter();

  /**
   * 析构函数
   */
  virtual ~KalmanFilter();

  /**
   * 初始化卡尔曼滤波器
   * @param x_in Initial state
   * @param P_in Initial state covariance
   * @param F_in Transition matrix
   * @param H_in Measurement matrix
   * @param R_in Measurement covariance matrix
   * @param Q_in Process covariance matrix
   */
  void Init(Eigen::VectorXd &x_in, Eigen::MatrixXd &P_in, Eigen::MatrixXd &F_in,
      Eigen::MatrixXd &H_in, Eigen::MatrixXd &R_in, Eigen::MatrixXd &Q_in);

  /**
   * 预测状态变量和状态协方差矩阵
   * 使用process model
   * @param delta_T Time between k and k+1 in s
   */
  void Predict();


  /**
   *  更新状态使用标准卡尔曼滤波器方程
   * @param z The measurement at k+1
   */
  void UpdateEKF(const Eigen::VectorXd &z);

};

#endif /* KALMAN_FILTER_H_ */
