#include <math.h>
#include "include/radarGUI/kalman_filter.h"


using Eigen::MatrixXd;
using Eigen::VectorXd;



const double PI2 = 2 * M_PI;

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}


/**
 * @brief KalmanFilter::Predict
 * @state:用上一时刻的值进行预测
 */

void KalmanFilter::Predict() {
  /**
  TODO:
    *预测状态变量
  */
  x_ = F_ * x_;
  MatrixXd Ft = F_.transpose();  //F的转置矩阵
  P_ = F_ * P_ * Ft + Q_;
  //qDebug() << "x_: " <<  x_(0) << x_(1) << x_(2)<<x_(3) <<endl;
}


/**
 * @brief KalmanFilter::UpdateEKF
 * @param z
 * state:用当前时刻的测量值更新
 */
void KalmanFilter::UpdateEKF(const VectorXd &z) {

  /**
    * 使用卡尔曼滤波方程更新状态变量
  */

  //将雷达测量值从笛卡尔坐标系(x,y,vx,vy)变换到极坐标系(rho,phi,rho_dot)

  VectorXd z_pred = tools1.RadarCartesianToPolar(x_);
  VectorXd y = z - z_pred;



  //将角度限制在-pi,到pi之间
  while(y(1) > M_PI){
    y(1) -= PI2;
  }

  while(y(1) < -M_PI){
    y(1) += PI2;
  }

  MatrixXd Ht = H_.transpose();
  MatrixXd PHt = P_ * Ht;
  MatrixXd S = H_ * PHt + R_;
  MatrixXd Si = S.inverse();
  MatrixXd K = PHt * Si;

  //新的估计值
  x_ = x_ + (K * y);
 long x_size = x_.size();
  MatrixXd I = MatrixXd::Identity(x_size, x_size);
  P_ = (I - K * H_) * P_;
}
