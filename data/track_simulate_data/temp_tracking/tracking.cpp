#include "Eigen/Dense"
#include <iostream>
#include "include/radarGUI/tracking.h"

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

Tracking::Tracking() {
  is_initialized_ = false;   //初始化标志
  previous_timestamp_ = 0;

  //创建一个4维的状态向量，x,y,vx,vy
  kf_.x_ = VectorXd(4);

  //状态协方差矩阵P
  kf_.P_ = MatrixXd(4, 4);
  kf_.P_ << 1, 0, 0, 0,
            0, 1, 0, 0, 
            0, 0, 1000, 0,
            0, 0, 0, 1000;


  //measurement covariance
  kf_.R_ = MatrixXd(2, 2);
  kf_.R_ << 0.0225, 0,
            0,      0.0225;

  //measurement matrix
  kf_.H_ = MatrixXd(2, 4);
  kf_.H_ << 1, 0, 0, 0,
            0, 1, 0, 0;

  //the initial transition matrix F_
  kf_.F_ = MatrixXd(4, 4);
  kf_.F_ << 1, 0, 1, 0,
            0, 1, 0, 1,
            0, 0, 1, 0,
            0, 0, 0, 1;

  //set the acceleration noise components
  noise_ax = 5;
  noise_ay = 5;

}


Tracking::~Tracking() {

}



// 新输入的测量值进行跟踪

void Tracking::ProcessMeasurement(const MeasurementPackage &measurement_pack) {

  //检查是否初始化，如果没有就将位置的初始值附给它，并且将速度赋值为0
  if (!is_initialized_) {
    //cout << "Kalman Filter Initialization " << endl;
    kf_.x_ << measurement_pack.raw_measurements_[0], measurement_pack.raw_measurements_[1], 0, 0;
    previous_timestamp_ = measurement_pack.timestamp_;
    is_initialized_ = true;
    return;
  }

  //求上次测量值与本次测量值之间的时间差
  float dt = (measurement_pack.timestamp_ - previous_timestamp_) / 1000000.0;	//dt - expressed in seconds
  previous_timestamp_ = measurement_pack.timestamp_;

  //计算一些过程的中间量
  float dt_2 = dt * dt;
  float dt_3 = dt_2 * dt;
  float dt_4 = dt_3 * dt;

  //Modify the F matrix so that the time is integrated
  kf_.F_(0, 2) = dt;
  kf_.F_(1, 3) = dt;

  //set the process covariance matrix Q
  kf_.Q_ = MatrixXd(4, 4);
  kf_.Q_ <<  dt_4/4*noise_ax, 0,               dt_3/2*noise_ax, 0,
             0,               dt_4/4*noise_ay, 0,               dt_3/2*noise_ay,
             dt_3/2*noise_ax, 0,               dt_2*noise_ax,   0,
             0,               dt_3/2*noise_ay, 0,               dt_2*noise_ay;

  //基于上一次的卡尔曼估计值得到的预测值
  kf_.Predict();

  //基于本次测量值得到的本次卡尔曼估计值
  kf_.Update(measurement_pack.raw_measurements_);

  //std::cout << "x_= " << kf_.x_ << std::endl;
  //std::cout << "P_= " << kf_.P_ << std::endl;

}

