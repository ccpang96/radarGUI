#include "include/radarGUI/track.h"
#include <iostream>
using Eigen::MatrixXd;
using Eigen::VectorXd;
using Eigen::VectorXi;
using Eigen::Vector3i;
using Eigen::Vector4i;
/**
  TODO:定义一个函数对vector进行升序排列
 */
bool less_sort(TrackDataOutputPackage a,TrackDataOutputPackage b) {return (a.raw_track_data_output_(0) < b.raw_track_data_output_(0)); }
bool less_sort_d(TrustTrackPackage a,TrustTrackPackage b) {return (a.raw_track_data_output_(0) < b.raw_track_data_output_(0)); }

/**
 * @brief Track::Track
 */
Track::Track() {

}

/**
 * @brief track  :航迹管理函数
 * @param measurement_pack_list_ :经过CFAR后的目标数据，内有多个目标
 * @param track_data_output_pack_list: 数据处理后，最终的输出
 * @param temp_point_pack_list_ :临时点迹数据
 * @param trust_track_pack_list_ :可靠航迹数据
 * @param number_frame_ :第几帧数据(批)
 * @param number_of_track : 第几条航迹
 * @param dt :两帧(批)数据之间的间隔
 * @param time_accumulate_ : 从第一批数据到now积累的时间
 */
void  Track::track_object() {

    //float r_min = 0;   //真实目标的距离门限值
    //float r_max = 10000;
    double phi_min = static_cast<double>(-M_PI); //目标的方位角门限值
    double phi_max = static_cast<double>(M_PI); //目标方位角的门限值
   // float r_dot_min = 0; //真实目标的速度门限值
   // float r_dot_max = 10000;


    int DBF_inform = 1; //本项目中未用到通道号，即将所有通道号都设置为1
    double K_start = 1; //控制起始波门大小的系数，本程序中起始波门取常数
    double K_association = 40; //控制相关波门大小的系数
    int gate_of_point_delete = 5; //点迹消除的门限次数 5

    size_t K = measurement_pack_list_.size(); //经过CFAR以后的目标数据
    size_t N = temp_point_pack_list_.size(); //临时点迹数据大小
    size_t H = trust_track_pack_list_.size(); //可靠航迹数据包中的大小
  //  qDebug() << "trust_track_pack_list's size " << H << endl;
  //  qDebug() << "temp_point_pack_list's size " << N << endl;
  //  qDebug() << "measurement_pack_list_'s size " << K << endl;
  //  cout << "measurement" << measurement_pack_list_[0].raw_measurements_ << endl;  //打印测量值看下
  //  qDebug() << "track_data_output_pack_list_ : " << track_data_output_pack_list_.size() << endl;

     /**先要判断新来的点是否为空*/
    if (K > 0) {  //经过CFAR以后的目标数据不为空
        /**
         * TODO:数据预处理，后续这个判断还需要优化
         */
        auto itm = measurement_pack_list_.begin();
        while(itm < measurement_pack_list_.end()) {
            if((*itm).raw_measurements_(1) < phi_min || (*itm).raw_measurements_(1) > phi_max) { //如果方位角不满足条件，删除
                itm = measurement_pack_list_.erase(itm);
            } else {
                ++itm;
            }
        }
       // qDebug() << "number_frame_: " << number_frame_ << endl;
       //qDebug() << "measurement_pack_list 's size:" << measurement_pack_list_.size() <<endl;
        size_t M = measurement_pack_list_.size(); //数据预处理后的目标个数
        qDebug() << "目标个数：" << M << endl;

        TempPointPackage temp_point; //临时点迹数据 中间变量
        temp_point.raw_temp_point_= Vector4i(4);
        NowFramePoint  now_point;    //当前点迹数据 中间变量
        now_point.raw_now_frame_point_ = Vector4i(4);
        TempPointPackage  temp_point1; //临时点迹数据  中间变
        temp_point1.raw_temp_point_= Vector4i(4);

        /**
          TODO:如果是第一批数据，就将它放入临时点迹中
        */
        if (number_frame_ == 1) {  //处理的是第一帧(批)数据
            time_accumulate_ = dt_; //时间积累
            if(M < 0.01) { // 第一批，经过数据预处理后的目标个数为0 说明第一批数据为空，接着处理下一批
                //此处要做什么
            } else { //第一批，输入数据不为空，存入临时点迹数据
                int number_of_unuse_point = 1; //点迹未用次数
                int flag_of_loop = 0; //航迹起始中，已用/未用标志，0表示未用
                for(size_t i = 0; i < M; ++i) { //数据预处理后的目标个数
                    temp_point.temp_point_ = measurement_pack_list_[i].raw_measurements_;
                    temp_point.time_accumulate_ = time_accumulate_;
                    temp_point.raw_temp_point_ << DBF_inform,number_of_unuse_point,gate_of_point_delete,flag_of_loop;  //通道号、点迹未使用次数、点迹消亡门限值、已用/未用
                    temp_point_pack_list_.emplace_back(std::move(temp_point));
                }
            }
           // qDebug() << "first frame temp_point_pack_list_.size()" << temp_point_pack_list_.size() << endl;
            qDebug() << "first frame is process complete!" << endl;

        } else {  //如果输入的不是第一批数据

            qDebug() << " 第 " << number_frame_ << " 批数据" << endl;
            time_accumulate_ += dt_; //时间积累
            /**
              TODO:输入数据为空，让临时点迹数据未使用次数+1；且让可靠航迹消亡和补点
            */
            if (M < 0.01) {  //不是第一批，且经过数据预处理后的目标个数为0
                if (N > 0) { //临时点迹数据不为空,但是因为此帧数据中没有目标，所以无法进行点迹关联，只能让临时点迹数据未使用次数+1
                    auto itc = temp_point_pack_list_.begin();
                    while(itc < temp_point_pack_list_.end()) {
                        (*itc).raw_temp_point_(1) += 1;   //未使用次数+1
                        if((*itc).raw_temp_point_(1) == (*itc).raw_temp_point_(2)) { //若点迹未使用次数等于删除门限
                            itc = temp_point_pack_list_.erase(itc); //删除它
                        } else {
                            ++itc;
                        }
                    }
                 }
                if( H > 0) { //如果可靠航迹非空,就调用航迹消亡函数和补点函数

                    /**
                     * TODO: 后续考虑交换这两个函数的执行顺序，是先补点还是先消亡，私以为是先补点，不过师兄的仿真程序是先消亡
                     */
                    point_supplement(); //调用航迹补点函数
                    track_die_out(); //调用航迹消亡函数
                }
            } else { //不是第一批，且经过数据预处理后存在目标
                int number_of_unuse_point = 1; //点迹未用次数
                int flag_of_loop = 0; //航迹起始中，已用/未用标志，0表示未用
                for(size_t i = 0; i < M; ++i) {
                    now_point.now_point_ = measurement_pack_list_[i].raw_measurements_;
                    now_point.time_accumulate_ = time_accumulate_;  //积累时间
                    now_point.raw_now_frame_point_<< DBF_inform,number_of_unuse_point,gate_of_point_delete,flag_of_loop; //通道号、点迹未用次数,
                    now_frame_point_pack_list_.push_back(now_point); //保存在待处理数据now_frame_point_pack_list_中
                }
                 //qDebug() << "now_frame_point_pack_list_" << now_frame_point_pack_list_.size() << endl;

                if (H > 0) { //如果可靠航迹非空
                    //存在待处理数据，先进行点迹航迹关联过程，然后判断哪条航迹消亡，然后看哪些航迹未更新，进行补点。然后看哪些航迹未更新次数等于门限值，消亡。
                    point_track_association(K_association);
                    track_die_out(); //调用航迹消亡
                    point_supplement(); //调用航迹补点函数
                   // qDebug() << " H > 0"  << endl;
                }
                if(N < 0.01) { //如果临时点迹为空，将点迹航迹关联后剩余的点迹now_frame_point_pack_list_放入临时点迹temp_point_pack_list_
                    size_t J = now_frame_point_pack_list_.size();
                    for (size_t j = 0 ;j < J; ++j ) {
                        temp_point1.temp_point_ = now_frame_point_pack_list_[j].now_point_;
                        temp_point1.raw_temp_point_ = now_frame_point_pack_list_[j].raw_now_frame_point_;
                        temp_point1.time_accumulate_ = now_frame_point_pack_list_[j].time_accumulate_;
                        temp_point_pack_list_.push_back(temp_point1); //将now点迹放入temp点迹   
                    }
                  // qDebug() << "N < 0.01"  << endl;
                } else { //否则就执行航迹起始，看看能否构成新的航迹
                    //qDebug() << "N > 0.01"  << endl;
                    track_start(K_start);
                } //如果临时点迹非空
            }
        }
    } else {  //如果经过CFAR后的目标为空
        if(number_frame_ == 1) { //如果是第一批数据
            time_accumulate_ = 0;  //时间积累为0
        }
        else { //不是第一批数据
            time_accumulate_ += dt_;
            if (N > 0) { //临时点迹数据不为空
                auto itc = temp_point_pack_list_.begin();
                while(itc < temp_point_pack_list_.end()) {
                    (*itc).raw_temp_point_(1) += 1;   //未使用次数+1
                    if((*itc).raw_temp_point_(1) == (*itc).raw_temp_point_(2)) { //若点迹未使用次数等于删除门限
                        itc = temp_point_pack_list_.erase(itc); //删除它
                    } else {
                        ++itc;
                    }
                }
             }//临时点迹数据非空

            if( H > 0) { //如果可靠航迹非空

                track_die_out(); //调用航迹消亡函数
                point_supplement(); //调用航迹补点函数
            }
        } //如果不是第一批数据
    } //恒虚警处理后的数据为空

    /**
      TODO:一批数据处理结束后，所有航迹更新标志置0
    */
    size_t S = trust_track_pack_list_.size();
    if (S > 0) {
        for (size_t s = 0; s < S; ++s) {
            trust_track_pack_list_[s].raw_trust_track_(1) = 0; //所有航迹更新标志置0
        }

        sort(track_data_output_pack_list_.begin(), track_data_output_pack_list_.end(),  less_sort); //按航迹标号升序排列
        sort(trust_track_pack_list_.begin(), trust_track_pack_list_.end(),  less_sort_d); //按航迹标号升序排列
    }


}

/**
 * @brief track_start
 * 说明：航迹起始函数
 * 功能：采用滑窗法，将满足一定要求的两个点迹关联成一条航迹，如果某一个点迹和temp_point_pack_list_中的点迹
 * 关联上了，那么它不能再与其他点迹进行关联，以保证每个点迹只使用一次。
 * 说明：track_start在临时点迹数据与当前帧经过点迹航迹关联后的剩余点迹数据中进行，依次查询两个数据package中的
 * 点迹，当两个点迹各方面之差都满足波门大小时，认为航迹起始成功，并初始化卡尔曼滤波器，将起始成功的航迹进行存储。
 * @param temp_point_pack_list_ 临时点迹数据
 * @param trust_track_pack_list_ 可靠航迹数据
 * @param track_data_output_pack_list 本帧数据处理完成后，输出的航迹数据
 * @param K_start  调整航迹起始波门的大小
 * @param time_accumulate_ 从第一批数据到此批处理数据之间所经过的时间
 * @param dt 每两批数据之间的间隔时间：
 * @param sigma_r 距离向测量噪声的标准差   显然应该写一个矩阵的 就是测量噪声协方差矩阵R_radar_
 * @param sigma_a 方位角测量噪声标准差
 * @param sigma_e 俯仰角测量噪声标准差
 * @param number_of_track 已形成的航迹数量
 */
void Track::track_start(double K_start ) {

    //qDebug() << "enter track_start function";

    int number_of_point = 1; //是该航迹中的第一个点
    int flag_of_come_go = 0; //0来/1去标志 //本项目中未用到来去标志
    int flag_of_true_void = 0; //0实点/1虚点
    int flag_of_renew = 0; // 航迹更新标志 0未更新1更新
    int number_of_unrenew = 0; //航迹未被更新次数

    /**
     * TODO:这些波门的大小必然是要修改的
     */
    double r_gate_of_start = K_start * 400; //航迹起始中的距离波门大小  模拟时使用400
    double phi_gate_of_start = K_start * 0.00209789; //航迹起始中的角度波门大小
    double r_dot_gate_of_start = K_start * 30; //航迹起始中的速度波门大小

    FusionEKF ts_EKF;
    Eigen::Vector4d first_point_x_; //新航迹笛卡尔坐标系下的第一点

    /**
     * TODO：这个P要修改，因为是初始化卡尔曼滤波器
     */
    Eigen::Matrix4d first_point_P_ = MatrixXd(4, 4);
    first_point_P_ <<
         1, 0, 0, 0,
         0, 1, 0, 0,
         0, 0, 1, 0,
         0, 0, 0, 1;

    TrackDataOutputPackage temp_track_dataout;
    temp_track_dataout.raw_track_data_output_ = VectorXi(5);

    TrustTrackPackage temp_trust;
    temp_trust.raw_track_data_output_ = VectorXi(5);
    temp_trust.raw_trust_track_ = Vector3i(3);

    size_t M = now_frame_point_pack_list_.size();  //当前帧中待处理的点迹
    size_t N = temp_point_pack_list_.size();  //临时点迹大小
    //qDebug() << "point and track association success!";
    //qDebug() << "now_frame_point_pack_list_ " << M << endl;
    //qDebug() << "temp_point_pack_list_" << N << endl;
    for (size_t m = 0; m < M ; ++m) {
        for (size_t n = 0; n < N; ++n) {

            if((now_frame_point_pack_list_[m].raw_now_frame_point_(3) == 0) && (temp_point_pack_list_[n].raw_temp_point_(3) == 0)) {  //如果该点没有被查询

                if(abs(now_frame_point_pack_list_[m].now_point_(0) - temp_point_pack_list_[n].temp_point_(0)) <= r_gate_of_start
                        && abs(now_frame_point_pack_list_[m].now_point_(1) - temp_point_pack_list_[n].temp_point_(1)) <= phi_gate_of_start
                               && abs(now_frame_point_pack_list_[m].now_point_(2) - temp_point_pack_list_[n].temp_point_(2)) <= r_dot_gate_of_start) {
                    qDebug() << "new track_start success!" << endl;;

                    qDebug() <<  now_frame_point_pack_list_[m].now_point_(0) << "  "<<
                                 temp_point_pack_list_[n].temp_point_(0) << "  " <<
                                 now_frame_point_pack_list_[m].now_point_(1) << "  "<<
                                 temp_point_pack_list_[n].temp_point_(1) << "  " <<
                                 now_frame_point_pack_list_[m].now_point_(2) << "  "<<
                                 temp_point_pack_list_[n].temp_point_(2) <<endl;
                    qDebug() << " distance cha: " << abs(now_frame_point_pack_list_[m].now_point_(0) - temp_point_pack_list_[n].temp_point_(0)) << "  distance gate:  " << r_gate_of_start
                            << "phi cha " << abs(now_frame_point_pack_list_[m].now_point_(1) - temp_point_pack_list_[n].temp_point_(1)) <<"phi gate  "<<  phi_gate_of_start <<
                                   "speed cha: " << abs(now_frame_point_pack_list_[m].now_point_(2) - temp_point_pack_list_[n].temp_point_(2)) << "speed gate: " << r_dot_gate_of_start << endl;

                    //点迹与点迹关联成功，航迹起始
                    temp_point_pack_list_[n].raw_temp_point_(3) = 1; //临时点迹中该点已经被使用
                    now_frame_point_pack_list_[m].raw_now_frame_point_(3) = 1; //当前帧点迹中该点已被使用
                    number_of_track_ += 1; //形成的可靠航迹数+1 也就是航迹标号

                    /**
                      @brief:第一步：利用前两点初始化扩展卡尔曼滤波器
                    */

                    /*
                    qDebug() <<"qian "<< temp_point_pack_list_[n].temp_point_(0) \
                             << temp_point_pack_list_[n].temp_point_(1) \
                             << temp_point_pack_list_[n].temp_point_(2);
                     */
                    first_point_x_ = tools2.RadarPolarToCartesian(temp_point_pack_list_[n].temp_point_);  //将航迹中的第一个点转到笛卡尔坐标系
                    //std::cout << first_point_x_ << endl;
                    ts_EKF.onlyPredict(first_point_x_,first_point_P_,dt_); //先利用第一点进行一次初始化

                    //将第一个点迹信息push进track_data_output_pack_list
                    temp_track_dataout.track_dataout_point_ = tools2.RadarCartesianToPolar(ts_EKF.ekf_.x_);  //新航迹第一个的点迹信息
                    /*
                    qDebug() << "one predict:"<< temp_track_dataout.track_dataout_point_(0) \
                             << temp_track_dataout.track_dataout_point_(1) \
                             << temp_track_dataout.track_dataout_point_(2);
                             */
                    //分别是 航迹标识、来去、是第几个点、实点/虚点，通道号
                    temp_track_dataout.raw_track_data_output_ << number_of_track_,flag_of_come_go,number_of_point,flag_of_true_void,temp_point_pack_list_[n].raw_temp_point_(0);
                    temp_track_dataout.time_accumulate_ = temp_point_pack_list_[n].time_accumulate_;

                    track_data_output_pack_list_.push_back(temp_track_dataout); //将新航迹中的第一个点加入航迹信息


                    ts_EKF.ProcessMeasurement(now_frame_point_pack_list_[m].now_point_); //再利用第2点进行一次更新


                    //然后将第二个点迹信息push进track_data_output_pack_list
                    temp_track_dataout.track_dataout_point_ = tools2.RadarCartesianToPolar(ts_EKF.ekf_.x_);; //新航迹第2个点的点迹信息
                    //分别是 航迹标识、来去、是第几个点、实点/虚点，通道号
                    temp_track_dataout.raw_track_data_output_ << number_of_track_, flag_of_come_go,number_of_point + 1,flag_of_true_void,now_frame_point_pack_list_[m].raw_now_frame_point_(0);
                    temp_track_dataout.time_accumulate_  = now_frame_point_pack_list_[m].time_accumulate_;
                    track_data_output_pack_list_.push_back(temp_track_dataout);

                    //更新新航迹在可靠航迹中的最后一个点的信息
                    temp_trust.last_point_x_ = ts_EKF.ekf_.x_; //状态变量信息
                    temp_trust.last_point_P_ = ts_EKF.ekf_.P_; //最后一个点的协方差矩阵信息
                    temp_trust.raw_track_data_output_ << number_of_track_,flag_of_come_go,number_of_point + 1,flag_of_true_void,now_frame_point_pack_list_[m].raw_now_frame_point_(0);
                    //分别是航迹未被实测数据更新次数，航迹更新标志，航迹消亡门限值
                    temp_trust.raw_trust_track_ << number_of_unrenew,flag_of_renew,now_frame_point_pack_list_[m].raw_now_frame_point_(2);

                    trust_track_pack_list_.push_back(temp_trust);  //更新可靠信息

                }//如果距离、角度、速度都位于起始门限内，说明航迹起始成功
            }
        }//遍历临时点迹数据中的所有点迹
    }//遍历当前帧中待处理的所有点迹

    /**
      @brief:删除当前帧点迹数据和临时点迹数据中已经使用过的数据，并将临时点迹数据中剩余数据的未被使用次数+1，然后将输入点迹信息放入临时点迹信息中。
    */



    //删除当前帧待处理点迹中的已关联的点迹
    auto itn = now_frame_point_pack_list_.begin();
    while(itn < now_frame_point_pack_list_.end() ) {
        if ((*itn).raw_now_frame_point_(3) == 1) { //如果点迹已用,删除它
            itn = now_frame_point_pack_list_.erase(itn);
        } else {
            ++itn;
        }
    }

   // qDebug() << "after track_start: now_frame_point_pack_list's size: " << now_frame_point_pack_list_.size() << endl;
    //删除临时点迹中已经关联的点迹
    auto itt = temp_point_pack_list_.begin();
    while(itt < temp_point_pack_list_.end() ) {
        if ((*itt).raw_temp_point_(3) == 1) { //如果点迹已用,删除它
            itt = temp_point_pack_list_.erase(itt);
        } else {
            ++itt;
        }
    }

   // qDebug() << "after track_start: temp_point_pack_list_'s size: " << temp_point_pack_list_.size() << endl;

    //处理临时点迹数据中剩余的点迹信息
    auto itc = temp_point_pack_list_.begin();
    while(itc < temp_point_pack_list_.end()) {
        (*itc).raw_temp_point_(1) += 1;   //未使用次数+1
        if((*itc).raw_temp_point_(1) == (*itc).raw_temp_point_(2)) { //若点迹未使用次数等于删除门限
            itc = temp_point_pack_list_.erase(itc); //删除它
        } else {
            ++itc;
        }
    }

    TempPointPackage temp_point;
    temp_point.raw_temp_point_ = Vector4i(4);
    //将当前输入点迹中未关联上的点迹数据送入临时点迹数据中
    size_t H = now_frame_point_pack_list_.size();
    //qDebug() << "after track_start: now_frame_point_pack_list_'s size: " << now_frame_point_pack_list_.size() << endl;
    if (H > 0) {
        for (size_t i = 0 ; i < H; ++i) {


            temp_point.temp_point_ = now_frame_point_pack_list_[i].now_point_;
            temp_point.raw_temp_point_ = now_frame_point_pack_list_[i].raw_now_frame_point_;
            temp_point.time_accumulate_ = now_frame_point_pack_list_[i].time_accumulate_;

            temp_point_pack_list_.push_back(temp_point); //能不能用类强制转换
        }
    }

     // qDebug() << "track_start function success!";

}


/**
 * @brief point_track_association
 * name:点迹航迹关联函数
 * 功能：经过CFAR和数据预处理的数据与可靠航迹数据(trust_track_pack_list_)关联，能够关联上的，更新可靠航迹数据(trust_track_pack_list_),
 * 否则存入临时点迹数据(temp_point_pack_list_),用于航迹起始，或者最终成为噪声。
 * 一个点迹只能关联一条航迹，即最先与该点关联上的航迹，用该点来更新该航迹（细读一下，第一次读未知具体意思）
 *
 * 说明：点迹航迹关联在当前帧待处理数据(now_frame_point_pack_list_)与已经形成的可靠航迹（trust_track_pack_list_）之间进行，
 * 依次对每条航迹进行循环，查找关联上的点迹，并将点迹进行扩展卡尔曼滤波后更新航迹。
 * @param now_frame_point_pack_list_  经过CFAR和数据预处理后的当前帧待处理数据
 * @param trust_track_pack_list_      可靠航迹数据
 * @param track_data_output_pack_list  本帧(批)数据处理完毕后，输出的航迹数据
 * @param dt   两帧数据之间的时间差
 * @param K_association  可选参数，用于调节相关波门大小
 * @param time_accumulate_  积累时间，即从第一批数据到此批数据之间所经过的时间
 */

void Track::point_track_association(double K_association) {



   // qDebug() << "enter point_track_association " << endl;

    FusionEKF pta_EKF;
    double sigma_x_predict;
    double sigma_y_predict;

    double x_gate_of_association;  //x方向的相关波门
    double y_gate_of_association;   //y方向的相关波门

    TrackDataOutputPackage temp_track_dataout;
    temp_track_dataout.raw_track_data_output_ = VectorXi(5);

    VectorXd temp_now_point_Cartesian = VectorXd(4); //用于存放now_point从极坐标系变换到笛卡尔坐标下的临时变量

    size_t N = trust_track_pack_list_.size(); //可靠航迹个数
    size_t M = now_frame_point_pack_list_.size(); // 当前帧中待处理的点迹的数目
    for (size_t n = 0; n < N; ++n) {   //遍历可靠航迹
        for (size_t m = 0; m < M; ++m) { //遍历当前帧中待处理的点迹
            if (now_frame_point_pack_list_[m].raw_now_frame_point_(3) == 0 &&  //点迹未使用
                    trust_track_pack_list_[n].raw_trust_track_(1) == 0)  {      // 航迹未更新

                /**
                  @brief: 第一步：先计算出可靠航迹中最后一个点的扩展卡尔曼预测值
                */
                pta_EKF.onlyPredict(trust_track_pack_list_[n].last_point_x_, trust_track_pack_list_[n].last_point_P_,dt_); //航迹上最后一个点的状态变量和协方差矩阵
                /**
                  @brief: 第二步：计算相关波门
                */
                sigma_x_predict = pta_EKF.ekf_.P_(0,0); //x方向的协方差
                sigma_y_predict = pta_EKF.ekf_.P_(1,1); //y方向的协方差
                //qDebug() << "sigma_x_predict" << sigma_x_predict << "\t" << "sigma_y_predict" << sigma_y_predict << endl;

                //  TODO:注意此处这个波门与仿真值相比，没有加入测量值的噪声R  如果后续需要调节应该修改此处

                x_gate_of_association = K_association*sqrt(pow(sigma_x_predict,2) + (pow(pta_EKF.noise_ax,2) * pow(dt_,4) / 4));
                y_gate_of_association = K_association*sqrt(pow(sigma_y_predict,2) + (pow(pta_EKF.noise_ay,2) * pow(dt_,4) / 4));

               qDebug() << "x_gate_of_association" << x_gate_of_association << "\t" << "y_gate_of_association" << y_gate_of_association << endl;


                /**
                  TODO: 第三步：找到在波门内的当前帧中的目标点，对其进行扩展卡尔曼滤波
                */
                temp_now_point_Cartesian = tools2.RadarPolarToCartesian(now_frame_point_pack_list_[m].now_point_);
                qDebug() << "pta_EKF.ekf_.x_(0) - temp_now_point_Cartesian(0) " << pta_EKF.ekf_.x_(0) - temp_now_point_Cartesian(0)<<endl;
                qDebug() << "pta_EKF.ekf_.x_(1) - temp_now_point_Cartesian(1)" << pta_EKF.ekf_.x_(1) - temp_now_point_Cartesian(1)<<endl;
                if (abs(pta_EKF.ekf_.x_(0) - temp_now_point_Cartesian(0)) <= x_gate_of_association &&
                        abs(pta_EKF.ekf_.x_(1) - temp_now_point_Cartesian(1)) <= y_gate_of_association) {  //点迹航迹关联成功
                        //qDebug() << "point is association with track" <<endl;
                        now_frame_point_pack_list_[m].raw_now_frame_point_(3) = 1; // 该点被关联，已用

                        /**
                          TODO:对其做扩展卡尔曼估计
                        */
                        pta_EKF.ProcessMeasurement(now_frame_point_pack_list_[m].now_point_);
                        trust_track_pack_list_[n].last_point_x_ = pta_EKF.ekf_.x_; //更新航迹中最后一个点状态
                        trust_track_pack_list_[n].last_point_P_ = pta_EKF.ekf_.P_; //更新航集中最后一个点的协方差矩阵
                        trust_track_pack_list_[n].raw_track_data_output_(2) += 1;  // 这个点是这条航迹的第几个点  +1
                        trust_track_pack_list_[n].raw_track_data_output_(3) = 0;  //实点/补点标志，实点为0，补点为1
                        trust_track_pack_list_[n].raw_trust_track_(0) = 0;  //航迹未被实点更新次数置0
                        trust_track_pack_list_[n].raw_trust_track_(1) = 1;  //更新标志为1
                        trust_track_pack_list_[n].raw_trust_track_(2) = now_frame_point_pack_list_[m].raw_now_frame_point_(2); //更改删除门限

                        /**
                            TODO:如果不是刚起始的航迹，则输出将点迹加入到track_data_output_pack_list
                        */
                        if (trust_track_pack_list_[n].raw_track_data_output_(2) > 2) {//判断该点是该航迹的第几个点，如果大于2说明不是刚起始的航迹

                            //此处应该将扩展卡尔曼的估计值放入航迹信息Track_data_output_list，而且应该是将4维的估计值从笛卡尔坐标系变换到极坐标系
                            temp_track_dataout. track_dataout_point_ = tools2.RadarCartesianToPolar(trust_track_pack_list_[n].last_point_x_);
                            temp_track_dataout.time_accumulate_ = time_accumulate_;  //时间积累
                            temp_track_dataout.raw_track_data_output_ =  trust_track_pack_list_[n].raw_track_data_output_; //航迹属性
                            track_data_output_pack_list_.push_back(temp_track_dataout); //输出航迹
                        }
                } //点迹与航迹相关成功
            } //如果当前帧的目标点未用，且可靠航迹也未更新
        } //遍历当前帧数据包中的目标点
    }  //遍历可靠航迹数据包内的航迹


    /**
      TODO:删除与可靠航迹关联上的点迹信息
    */
    auto itt = now_frame_point_pack_list_.begin();
    while(itt < now_frame_point_pack_list_.end() ) {
        if ((*itt).raw_now_frame_point_(3) == 1) { //如果点迹已用,删除它
            itt = now_frame_point_pack_list_.erase(itt);
        } else {
            ++itt;
        }
    }

    /**
      TODO:track_data_output_pack_list按照航迹标志号排序
    */
    sort(track_data_output_pack_list_.begin(), track_data_output_pack_list_.end(),less_sort); //升序排列
    //qDebug() << "point_track_association success" << endl;
}



/**
 * @brief point_supplement
 * name:航迹补点函数
 * 功能：当航迹没有得到实测点迹进行更新，则采取航迹补点方法，即利用航迹的最后一个点迹的扩展卡尔曼预测值作为更新点迹。
 * @param trust_track_pack_list_  可靠航迹数据
 * @param track_data_output_pack_list 当前帧数据处理完成后，输出的航迹数据
 * @param dt  两帧（批）数据之间的间隔时间
 * @param time_accumulate_ 积累时间，即从第一帧（批）数据到当前帧数据之间所经过的时间
 */
void Track::point_supplement() {

    //qDebug() << "enter point_supplement " << endl;

    FusionEKF ps_EKF;  //用于补点的扩展卡尔曼滤波类

    TrackDataOutputPackage no_updata_track_data_output;  // 中间变量

    /**
     * @brief 第一步：找出都是哪些航迹未被更新
     */
    size_t N = trust_track_pack_list_.size(); //航迹个数
    for (size_t k = 0; k < N; ++k) {
         if (trust_track_pack_list_[k].raw_trust_track_(1) == 0 ) { //航迹更新标志 = 0，即航迹未更新


             //采用卡尔曼预测，得到补点,并用补点更新航迹信息
             ps_EKF.onlyPredict(trust_track_pack_list_[k].last_point_x_, trust_track_pack_list_[k].last_point_P_, dt_);

             //更新trust_track_pack_list
             trust_track_pack_list_[k].last_point_x_ = ps_EKF.ekf_.x_;  //使用扩展卡尔曼预测值进行更新状态变量x_
             trust_track_pack_list_[k].last_point_P_ = ps_EKF.ekf_.P_;  //更新协方差矩阵P_

             trust_track_pack_list_[k].raw_track_data_output_(2) += 1; //航迹点数加一
             trust_track_pack_list_[k].raw_track_data_output_(3) = 1; //补点标志位 置1
             trust_track_pack_list_[k].raw_trust_track_(0) += 1; //航迹未用实测点更新次数加1
             trust_track_pack_list_[k].raw_trust_track_(1) = 1; //航迹更新标志 置1


             //将补点得到的状态变量先变换到极坐标中去
             no_updata_track_data_output.track_dataout_point_  = tools2.RadarCartesianToPolar(trust_track_pack_list_[k].last_point_x_); //航迹信息的更新点迹
             no_updata_track_data_output.time_accumulate_ = time_accumulate_; //积累时间
             no_updata_track_data_output.raw_track_data_output_ =  trust_track_pack_list_[k].raw_track_data_output_;

             //更新track_data_output_pack_list_
             track_data_output_pack_list_.emplace_back(no_updata_track_data_output);
         }
     }


    /**
     * @brief 第二步：对track_data_output_pack_list和trust_track_pack_list按照航迹标识进行排序
     */

    sort(track_data_output_pack_list_.begin(), track_data_output_pack_list_.end(),less_sort); //升序排列
    sort(trust_track_pack_list_.begin(),trust_track_pack_list_.end(),less_sort_d);
    //qDebug() << "point_supplement success" << endl;
}




/**
 * @brief track_die_out
 * name: 航迹消亡函数
 * 功能：当未用实测点更新次数达到该航迹消亡的门限值时，该条航迹消亡（为啥不叫死亡呢，小声哔哔）,即删除该条航迹的所有信息
 * @param trust_track_pack_list_   可靠航迹数据
 * @param track_data_output_pack_list  当前帧（批）数据处理完毕后，输出的航迹数据
 * @param member_of_track  已经形成的航迹个数
 */
void Track::track_die_out() {

   // qDebug() << "enter track_die_out" << endl;
    int die_track_index;
    vector<int>die_track_index_pack_list;  //已经消亡的的航迹识别标志
    /**
     * @brief 第一步：找出都是哪些航迹消亡。
     */
    size_t N = trust_track_pack_list_.size(); //可靠航迹个数
    for (size_t k = 0; k < N; ++k) {  //遍历可靠航迹数据中的每条航迹
         if (trust_track_pack_list_[k].raw_trust_track_(0) == trust_track_pack_list_[k].raw_trust_track_(2)) { //未被实测数据更新次数 = 航迹消亡门限值
             die_track_index = trust_track_pack_list_[k].raw_track_data_output_(0); //即将消亡的航迹的识别标志
             die_track_index_pack_list.push_back(die_track_index);
         }  //如果第k条航迹的未被更新次数值达到航迹消亡的未被更新次数的门限值
     }

     /**
    * @brief M 消亡的航迹的数目
    */
   size_t M = die_track_index_pack_list.size();
   for (size_t k = 0 ; k < M; ++k) {
       qDebug() << "track is die" << endl;
       /**
        * @brief 在track_data_output_pack_list中找到标识符为die_track_index_pack_list[k]的这条消亡航迹，然后删除它
        */
       auto ito = track_data_output_pack_list_.begin(); //it类型是vector<TrackDataOutputPackage>::iterator
       while(ito < track_data_output_pack_list_.end() ) { //注意使用while的技巧，因为一直在erase，所以vector的大小在一直变化
           if((*ito).raw_track_data_output_(0) == die_track_index_pack_list[k]) { //在track_data_output_pack_list中找到标识符为死了的航迹
               ito = track_data_output_pack_list_.erase(ito); //erase返回的是当前删除元素的下一个元素的迭代器
           } else {
               ++ito;
           }
       }

       /**
        * @brief 在trust_track_pack_list_中找到标识符为die_index_pack_list[k]的这条消亡航迹，然后删除它
        */
       auto itt = trust_track_pack_list_.begin();
       while(itt < trust_track_pack_list_.end() ) {
           if ((*itt).raw_track_data_output_(0) == die_track_index_pack_list[k]) {
               itt = trust_track_pack_list_.erase(itt);
           } else {
               ++itt;
           }
       }


       /**
        * @brief 如果消亡的不是最后一条航迹，需要将航迹号重新赋值
        */
       if (die_track_index_pack_list[k] < number_of_track_) {

           for (int cnt_index = die_track_index_pack_list[k] + 1; cnt_index < number_of_track_; ++cnt_index) {

               /**
                * @brief 修改可靠航迹数据trust_track_pack_list_中的航迹标识符
                */
               auto itt = trust_track_pack_list_.begin();
               while (itt < trust_track_pack_list_.end() ) {
                   if ((*itt).raw_track_data_output_(0) == cnt_index) //
                       (*itt).raw_track_data_output_(0) = cnt_index -1;
                    ++itt;
               }

               /**
                * @brief 修改航迹信息数据track_data_output_pack_list中的航迹标识符
                */

               auto ito = track_data_output_pack_list_.begin();
               while (ito < track_data_output_pack_list_.end() ) {
                   if((*ito).raw_track_data_output_(0) == cnt_index)
                       (*ito).raw_track_data_output_(0) = cnt_index -1;
                   ++ito;
               }
           } //遍历死了的第k条航迹后面的航迹，将它们的航迹标识符都减小1
       }

       /**
      * @brief 由于上面更新了航迹标号，所以也需要更新die_track_index_pack_list中的航迹标号
      */

       /**
         TODO:以后将j改为k+1试一下
       */
       for (size_t  j = k+1 ; j < M;  ++j ) { //注意此处j = 0;或者j = k +1;都可以
            die_track_index_pack_list[j] = die_track_index_pack_list[j] - 1;
       }
   }

   number_of_track_ = number_of_track_ - static_cast<int>(M); //更新一下已经形成的航迹的数目

     //qDebug() << "track_die_out success" << endl;
}


































