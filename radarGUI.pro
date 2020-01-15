#-------------------------------------------------
#
# Project created by QtCreator 2019-09-18T07:54:48
#
#-------------------------------------------------

QT       += core gui network
#QT       += serialport  #串口

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = radarGUI
TEMPLATE = app  #用于构建应用程序的Makefile

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS  #qmake添加作为编译器C预处理器宏

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0



CONFIG += c++11#

SOURCES += \
    package/shape/safileio.c \
    package/shape/shpopen.c \
    src/map/loader.cpp \
    src/map/map_data.cpp \
    src/map/shp.cpp \
    src/map/dbf.cpp \
    package/shape/dbfopen.c \
    src/map/china_loader.cpp \
    src/radarGUI/FusionEKF.cpp \
    src/radarGUI/kalman_filter.cpp \
    src/radarGUI/main.cpp \
    src/radarGUI/mainwindow.cpp \
    src/radarGUI/show.cpp \
    src/radarGUI/tools.cpp \
    src/radarGUI/track.cpp \
    src/setParameter/worker.cpp \
    src/setParameter/dbf_to_dam.cpp \
    src/setParameter/dbf_to_calib.cpp \
    src/setParameter/udpclient.cpp \
    src/setParameter/psp_tdb_plot_report.cpp


HEADERS += \
    include/radarGUI/mainwindow.h \
    include/radarGUI/show.h \
    include/radarGUI/FusionEKF.h \
    include/radarGUI/kalman_filter.h \
    include/radarGUI/tools.h \
    include/radarGUI/ground_truth_package.h \
    include/radarGUI/track_package.h \
    include/radarGUI/track.h \
    package/shape/shapefil.h \
    include/map/adj_entry.h \
    include/map/base_objects.h \
    include/map/loader.h \
    include/map/map_data.h \#
    include/map/shp.h \
    include/map/dbf.h \
    include/map/china_loader.h \
    include/setParameter/dbf_to_dam.h \
    include/setParameter/worker.h \
    include/setParameter/dbf_to_dam_define.h \
    include/setParameter/dbf_to_calib.h \
    include/setParameter/udpclient.h \
    include/setParameter/signal_to_data.h


FORMS += \
    mainwindow.ui

INCLUDEPATH += $$_PRO_FILE_PWD_/include/radarGUI/ \
               $$_PRO_FILE_PWD_/include/map \
               $$_PRO_FILE_PWD_/package/

#下面这个是默认的部署命令
# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#解决中文乱码问题
QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8

RESOURCES += \
    res/res.qrc

RC_ICONS = logo.ico
