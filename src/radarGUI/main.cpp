#include <QApplication>
#include <QStyleFactory>
#include "include/radarGUI/mainwindow.h"
#include <QString>
using Eigen::VectorXd;


/*
void check_files(ifstream& in_file, string& in_name,
                 ofstream& out_file, string& out_name) {
  if (!in_file.is_open()) {
    cerr << "Cannot open input file: " << in_name << endl;
    exit(EXIT_FAILURE);
  }

  if (!out_file.is_open()) {
    cerr << "Cannot open output file: " << out_name << endl;
    exit(EXIT_FAILURE);
  }
}


void  object_track() {


   string in_file_name_ = "F:\\radarGUI\\Docs\\Radar2RAE.txt";
   ifstream in_file_(in_file_name_.c_str(), ifstream::in);
   string out_file_name_0 = "F:\\radarGUI\\Docs\\output0.txt"; //用于存储目标0
   ofstream out_file_0(out_file_name_0.c_str(), ofstream::out);
   string out_file_name_1 = "F:\\radarGUI\\Docs\\output1.txt"; //用于存储目标1
   ofstream out_file_1(out_file_name_1.c_str(), ofstream::out);
   string out_file_name_2 = "F:\\radarGUI\\Docs\\output2.txt"; //用于存储目标1
   ofstream out_file_2(out_file_name_2.c_str(), ofstream::out);
   string out_file_name_3 = "F:\\radarGUI\\Docs\\output3.txt"; //用于存储目标1
   ofstream out_file_3(out_file_name_3.c_str(), ofstream::out);

   //检查文件打开是否正确
 //  check_files(in_file_, in_file_name_, out_file_, out_file_name_);

  vector<MeasurementPackage> measurement_pack_list;  //存放测量值的vector
  //vector<GroundTruthPackage> gt_pack_list;          //存放真实值的vector

  string line;
  static quint32 i0 = 0;
  static quint32 i1 = 0;
  static quint32 i2 = 0;
  static quint32 i3 = 0;

  while (getline(in_file_, line)) {

    istringstream iss(line);

    int number; //目标序号
    double timestamp;  //用于存放时刻值
    double ro;  //距离
    double phi; //角度
    double ro_dot = 0.0; //速度
    iss >> number;
    //读入测量值
    //MeasurementPackage meas_package;
    //meas_package.raw_measurements_ = VectorXd(3);  //用于存放点目标信息 为了验证跟踪效果，修改为5

    static double ro_last0; //上次的距离值
    static double timestamp_last0; //上次的时间戳
    static double ro_last1; //上次的距离值
    static double timestamp_last1; //上次的时间戳
    static double ro_last2; //上次的距离值
    static double timestamp_last2; //上次的时间戳
    static double ro_last3; //上次的距离值
    static double timestamp_last3; //上次的时间戳
//序号 时间 距离 方位 俯仰角
    if (number == 0) {
        i0 += 1;
        if(i0 == 1) {
          //说明是第一次进入，速度为0
          iss >> timestamp;   out_file_0 << timestamp << "\t";
          iss >> ro;          out_file_0 << ro << "\t";
          iss >> phi;         out_file_0 << phi << "\t";
          ro_dot = 0;         out_file_0 << ro_dot << "\n";
          ro_last0 = ro;       timestamp_last0 = timestamp;
        } else {
        //说明是第一次进入，速度为0
        iss >> timestamp;   out_file_0 << timestamp << "\t";
        iss >> ro;          out_file_0 << ro << "\t";
        iss >> phi;         out_file_0 << phi << "\t";
        ro_dot = (ro - ro_last0) / (timestamp - timestamp_last0);         out_file_0 << ro_dot << "\n";
        ro_last0 = ro;       timestamp_last0 = timestamp;
    }
  } else if (number == 1) {
        i1 += 1;
        if(i1 == 1) {
          //说明是第一次进入，速度为0
          iss >> timestamp;   out_file_1 << timestamp << "\t";
          iss >> ro;          out_file_1 << ro << "\t";
          iss >> phi;         out_file_1 << phi << "\t";
          ro_dot = 0;         out_file_1 << ro_dot << "\n";
          ro_last1 = ro;       timestamp_last1 = timestamp;
        } else {
        //说明是第一次进入，速度为0
        iss >> timestamp;   out_file_1 << timestamp << "\t";
        iss >> ro;          out_file_1 << ro << "\t";
        iss >> phi;         out_file_1 << phi << "\t";
        ro_dot = (ro - ro_last1) / (timestamp - timestamp_last1);         out_file_1 << ro_dot << "\n";
        ro_last1 = ro;       timestamp_last1 = timestamp;
    }
  } else if (number == 2) {
        i2 += 1;
        if(i2 == 1) {
          //说明是第一次进入，速度为0
          iss >> timestamp;   out_file_2 << timestamp << "\t";
          iss >> ro;          out_file_2 << ro << "\t";
          iss >> phi;         out_file_2 << phi << "\t";
          ro_dot = 0;         out_file_2 << ro_dot << "\n";
          ro_last2 = ro;       timestamp_last2 = timestamp;
        } else {
        //说明是第一次进入，速度为0
        iss >> timestamp;   out_file_2 << timestamp << "\t";
        iss >> ro;          out_file_2 << ro << "\t";
        iss >> phi;         out_file_2 << phi << "\t";
        ro_dot = (ro - ro_last2) / (timestamp - timestamp_last2);         out_file_2 << ro_dot << "\n";
        ro_last2 = ro;       timestamp_last2 = timestamp;
    }
  } else if (number == 3) {
        i3 += 1;
        if(i3 == 1) {
          //说明是第一次进入，速度为0
          iss >> timestamp;   out_file_3 << timestamp << "\t";
          iss >> ro;          out_file_3 << ro << "\t";
          iss >> phi;         out_file_3 << phi << "\t";
          ro_dot = 0;         out_file_3 << ro_dot << "\n";
          ro_last3 = ro;       timestamp_last3 = timestamp;
        } else {
        //说明是第一次进入，速度为0
        iss >> timestamp;   out_file_3 << timestamp << "\t";
        iss >> ro;          out_file_3 << ro << "\t";
        iss >> phi;         out_file_3 << phi << "\t";
        ro_dot = (ro - ro_last3) / (timestamp - timestamp_last3);         out_file_3 << ro_dot << "\n";
        ro_last3 = ro;       timestamp_last3 = timestamp;
    }
  }

}
  if (out_file_0.is_open()) {
    out_file_0.close();
  }
  if (out_file_1.is_open()) {
    out_file_1.close();
  }
  if (out_file_2.is_open()) {
    out_file_2.close();
  }
  if (out_file_3.is_open()) {
    out_file_3.close();
  }

  if (in_file_.is_open()) {
    in_file_.close();
  }

}

*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setStyle(QStyleFactory::create("Fusion"));

   // qDebug() << acos(0.5) << endl;;
    qDebug() << "The main threadID is :" << QThread::currentThreadId();


  //  object_track();
     MainWindow w;
     //w._mapdata->drawChina();
    //w.setWindowFlags(w.windowFlags()& ~Qt::WindowMaximizeButtonHint&  ~Qt::WindowMinimizeButtonHint);  //去除最小化和最大化图标，为了偷懒！！！
    w.show();  //窗口创建默认是隐藏的，需要通过调用show()才能显示出来
    return a.exec();   //程序运行停在这里，等待事件发生
}
