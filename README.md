<font size = 3>

radarGUI是一个雷达数据处理软件，它使用QT开发，主要包括界面显示、参数下发及数据接收、目标跟踪等部分。

## 界面显示
- 主要使用shapelib库读取shapefile文件绘制地图。
- 绘制圆形及刻度操作
- 通过定时器，增加了动态扫描的过程

## 参数下发及数据读取

- 参数下发通过UDP网络，设置需要配置的DAM及校正参数
- 主线程用于界面显示，第二个线程用于下发参数及接收数据
- 在接收数据时，开启buffer进行接收，加mutex锁保护该buffer

## 目标跟踪 
- 目标跟踪部封装成track类，主要包括航迹起始、点迹航迹关联、航迹消亡、航迹补点，滤波部分使用的是扩展卡尔曼滤波。

## 注意事项：
在china_loader.h文件中的macro，使用的是绝对路径，要修改为自己文件所在的位置。
    #define MOCT_CHINA_PATH  "E:\\radarGUI\\data\\china_basic_map\\bou2_4l"
## 界面显示如下：

[](https://github.com/ccpang96/radarGUI/blob/master/images/interface.png)

</font>
