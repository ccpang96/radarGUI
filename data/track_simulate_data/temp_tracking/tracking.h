
#ifndef FUSION_KF_H_
#define FUSION_KF_H_

#include "measurement_package.h"
#include <vector>
#include <string>
#include <fstream>
#include "kalman_filter.h"

class Tracking {
public:
	Tracking();
	virtual ~Tracking();
	void ProcessMeasurement(const MeasurementPackage &measurement_pack);
	KalmanFilter kf_;

private:
    bool is_initialized_;      //是否初始化的标志
    long previous_timestamp_;  //前一个的时间刻度值

    //acceleration noise components
	float noise_ax;
	float noise_ay;

};

#endif /* FUSION_KF_H_ */

