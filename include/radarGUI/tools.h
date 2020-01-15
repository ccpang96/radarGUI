#ifndef TOOLS_H_
#define TOOLS_H_
#include <vector>
#include "Eigen/Dense"
#include "QDebug"
#include <QHostAddress>


class Tools {
public:
  /**
  *构造函数
  */
  Tools();

  /**
  * 析构函数
  */
  virtual ~Tools();

  /**
  * 用于计算雅克比矩阵的函数
  */
  Eigen::MatrixXd CalculateJacobian(const Eigen::VectorXd& x_state);
  Eigen::VectorXd RadarCartesianToPolar(const Eigen::VectorXd &x_state);
  Eigen::VectorXd RadarPolarToGUI(const Eigen::VectorXd &resultEKF);
  Eigen::VectorXd RadarPolarToCartesian(const Eigen::VectorXd &x_measurement);
  ulong rotateLeft(ulong a, unsigned int n); //循环左移
  ulong rotateRight(ulong a, unsigned int n); //循环右移
  QHostAddress getLocalHostIP(); //获取本地IP地址
};

#endif /* TOOLS_H_ */
