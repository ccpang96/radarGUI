#include "include/radarGUI/FusionEKF.h"
#include "include/radarGUI/tools.h"
#include "Eigen/Dense"
#include <iostream>

using namespace std;
using Eigen::MatrixXd;
using Eigen::VectorXd;
using std::vector;

static Tools tools;

/**
 * 构造函数.
 */
FusionEKF::FusionEKF() {
  is_initialized_ = false;

  previous_timestamp_ = 0;

  // 初始化矩阵
  H_jacobian = MatrixXd(3, 4);

  /**
   * @参数说明：测量噪声协方差
   * @参数说明：滤波器实现时，测量噪声协方差R一般可以观测得到，是滤波器的已知条件。
   * @参数设置：
   */
  R_radar_ = MatrixXd(3, 3);
  R_radar_ << 4000, 0, 0,
              0, 0.0001, 0,
              0, 0, 0.01;
 /**
  TODO:
  *初始化卡尔曼滤波器参数
 */

  /**
   * @参数说明：后验估计协方差矩阵
   * @参数说明：
   * @参数设置：
   */
  ekf_.P_ = MatrixXd(4, 4);
  ekf_.P_ << 100, 0, 0, 0,
             0, 100, 0, 0,
             0, 0, 100, 0,
             0, 0, 0, 100;


  /**
   * @参数说明：状态转移矩阵
   * @参数说明：状态转移矩阵通常用于对目标的运动建模，当状态转移矩阵不符合目标的状态转换模型时，滤波很快会发散
   * @参数设置：
   */
  ekf_.F_ = MatrixXd(4, 4);
  ekf_.F_ << 1, 0, 1, 0,
             0, 1, 0, 1,
             0, 0, 1, 0,
             0, 0, 0, 1;

  /**
   * @参数说明：加速度噪声
   * @参数说明：
   * @参数设置：
   */
  noise_ax = 9;
  noise_ay = 9;
}

/**
* 析构函数
*/
FusionEKF::~FusionEKF() {}


/**
 * @brief FusionEKF::onlyPredict
 * @param last_point_x_
 * @param dt
 * 说明：这个函数是只进行扩展卡尔曼预测
 */
void FusionEKF::onlyPredict(const Eigen::Vector4d &last_point_x_, const Eigen::Matrix4d &last_point_P_, const double &dt) {


    /*****************************************************************************
    * 第一步：基于可靠航迹中的最后一个点进行扩展卡尔曼滤波预测
    ****************************************************************************/
    ekf_.x_ = VectorXd(4);
    ekf_.x_ = last_point_x_;
    ekf_.P_ = last_point_P_;

    double dt_2 = dt * dt;
    double dt_3 = dt_2 * dt;
    double dt_4 = dt_3 * dt;

    ekf_.F_(0, 2) = dt;    //根据匀加速运动模型 算出来的F的第0行第2列就是dt
    ekf_.F_(1, 3) = dt;    //第1行第3列也是dt

    ekf_.Q_ = MatrixXd(4, 4);
    ekf_.Q_ << dt_4/4*noise_ax,   0,                dt_3/2*noise_ax,  0,
               0,                 dt_4/4*noise_ay,  0,                dt_3/2*noise_ay,
               dt_3/2*noise_ax,   0,                dt_2*noise_ax,    0,
               0,                 dt_3/2*noise_ay,  0,                dt_2*noise_ay;

    ekf_.Predict();
}



void FusionEKF::ProcessMeasurement(const Eigen::Vector3d &point_now) {

/*****************************************************************************
* 第一步：基于本次的测量值进行更新
****************************************************************************/

  /**
    TODO:
     * 更新状态变量和协方差矩阵
   */

   H_jacobian = tools.CalculateJacobian(ekf_.x_);
   ekf_.H_ = H_jacobian;
   ekf_.R_ = R_radar_;
   ekf_.UpdateEKF(point_now);


    //  cout << "x_ = " << ekf_.x_ << endl;
    // cout << "P_ = " << ekf_.P_ << endl;
}
