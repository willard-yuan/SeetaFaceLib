# Author: yongyuan.name


QT       += core gui

CONFIG += c++11

QMAKE_CXXFLAGS += -msse -msse2 -msse3 -mssse3

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_MAC_SDK = macosx10.13

TARGET = CocsoFace
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    videohandler.cpp \
    helper.cpp \
    ../FaceLib/FaceAlignment/srcFA/cfan.cpp \
    ../FaceLib/FaceAlignment/srcFA/face_alignment.cpp \
    ../FaceLib/FaceAlignment/srcFA/sift.cpp \
    ../FaceLib/FaceDetection/srcFD/classifier/lab_boosted_classifier.cpp \
    ../FaceLib/FaceDetection/srcFD/classifier/mlp.cpp \
    ../FaceLib/FaceDetection/srcFD/classifier/surf_mlp.cpp \
    ../FaceLib/FaceDetection/srcFD/feat/lab_feature_map.cpp \
    ../FaceLib/FaceDetection/srcFD/feat/surf_feature_map.cpp \
    ../FaceLib/FaceDetection/srcFD/io/lab_boost_model_reader.cpp \
    ../FaceLib/FaceDetection/srcFD/io/surf_mlp_model_reader.cpp \
    ../FaceLib/FaceDetection/srcFD/util/image_pyramid.cpp \
    ../FaceLib/FaceDetection/srcFD/util/nms.cpp \
    ../FaceLib/FaceDetection/srcFD/face_detection.cpp \
    ../FaceLib/FaceDetection/srcFD/fust.cpp \
    src/extractFeats.cpp

HEADERS  += mainwindow.h \
    videohandler.h \
    helper.h \
    src/extractFeats.h

FORMS    += \
    mainwindow.ui

#QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10

# Boost include
INCLUDEPATH += /usr/local/include

# OpenCV include
INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include/opencv2

# Face include
INCLUDEPATH += ../FaceLib/FaceAlignment/include
INCLUDEPATH += ../FaceLib/FaceDetection/include
INCLUDEPATH += ../FaceLib/FaceIdentification/include

# Falconn include
INCLUDEPATH += ../3rd/FALCONN/src/include

# Eigen include
INCLUDEPATH += ../3rd/FALCONN/external/eigen

LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_videoio -lopencv_imgcodecs -lopencv_video -lopencv_photo -lboost_iostreams -lboost_serialization

# Face lib, need to set the full path
LIBS += /Users/willard/codes/cpp/face/SeetaFaceLib/FaceLib/FaceIdentification/lib/libviplnet.4.5.dylib

RESOURCES += design.qrc

DISTFILES += \
    icon.iconset/1473021726_eye.png \
    icon.iconset/1473021730_eye.png \
    icon.iconset/1473021733_eye.png \
    icon.iconset/1473021736_eye.png \
    icon.iconset/1473021739_eye.png \
    icon.iconset/1473021741_eye.png \
    icon.iconset/1473021744_eye.png \
    icon.iconset/icon_12x12.png \
    icon.iconset/icon_256x256.png \
    icon.icns \
    qss/blue.css \
    qss/brown.css \
    qss/dev.css \
    qss/lightgray.css \
    qss/MetroDark.css \
    qss/silvery.css
