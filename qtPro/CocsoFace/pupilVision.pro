#-------------------------------------------------
#
# Project created by QtCreator 2016-05-15T08:52:04
#
#-------------------------------------------------

QT       += core gui

CONFIG += c++11

QMAKE_CXXFLAGS += -msse -msse2 -msse3 -mssse3

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = pupilVision
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    videohandler.cpp \
    helper.cpp \
    ../../../cpp/face/SeetaFaceLib/FaceAlignment/srcFA/cfan.cpp \
    ../../../cpp/face/SeetaFaceLib/FaceAlignment/srcFA/face_alignment.cpp \
    ../../../cpp/face/SeetaFaceLib/FaceAlignment/srcFA/sift.cpp \
    ../../../cpp/face/SeetaFaceLib/FaceDetection/srcFD/classifier/lab_boosted_classifier.cpp \
    ../../../cpp/face/SeetaFaceLib/FaceDetection/srcFD/classifier/mlp.cpp \
    ../../../cpp/face/SeetaFaceLib/FaceDetection/srcFD/classifier/surf_mlp.cpp \
    ../../../cpp/face/SeetaFaceLib/FaceDetection/srcFD/feat/lab_feature_map.cpp \
    ../../../cpp/face/SeetaFaceLib/FaceDetection/srcFD/feat/surf_feature_map.cpp \
    ../../../cpp/face/SeetaFaceLib/FaceDetection/srcFD/io/lab_boost_model_reader.cpp \
    ../../../cpp/face/SeetaFaceLib/FaceDetection/srcFD/io/surf_mlp_model_reader.cpp \
    ../../../cpp/face/SeetaFaceLib/FaceDetection/srcFD/util/image_pyramid.cpp \
    ../../../cpp/face/SeetaFaceLib/FaceDetection/srcFD/util/nms.cpp \
    ../../../cpp/face/SeetaFaceLib/FaceDetection/srcFD/face_detection.cpp \
    ../../../cpp/face/SeetaFaceLib/FaceDetection/srcFD/fust.cpp

HEADERS  += mainwindow.h \
    videohandler.h \
    helper.h

FORMS    += \
    mainwindow.ui

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.10

INCLUDEPATH += /usr/local/include/opencv
INCLUDEPATH += /usr/local/include/opencv2
INCLUDEPATH += /usr/local/include

# face include
INCLUDEPATH += /Users/willard/codes/cpp/face/SeetaFaceLib/FaceAlignment/include
INCLUDEPATH += /Users/willard/codes/cpp/face/SeetaFaceLib/FaceDetection/include
INCLUDEPATH += /Users/willard/codes/cpp/face/SeetaFaceLib/FaceIdentification/include

LIBS += -L/usr/local/lib -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video -lopencv_photo -ltbb -ltbbmalloc -ltbbmalloc_proxy

# face lib
LIBS += /Users/willard/codes/cpp/face/SeetaFaceLib/FaceIdentification/lib/libviplnet.4.5.dylib

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
    MetroMumble/config_asio.png \
    MetroMumble/config_audio_input.png \
    MetroMumble/config_audio_output.png \
    MetroMumble/config_basic.png \
    MetroMumble/config_lcd.png \
    MetroMumble/config_msgs.png \
    MetroMumble/config_network.png \
    MetroMumble/config_osd.png \
    MetroMumble/config_plugin.png \
    MetroMumble/config_shortcuts.png \
    MetroMumble/config_ui.png \
    MetroMumble/mumble.osx.png \
    MetroMumble/mumble.png \
    MetroMumble/actions/audio-input-microphone-muted.svg \
    MetroMumble/actions/audio-input-microphone.svg \
    MetroMumble/actions/format-text-bold.svg \
    MetroMumble/actions/format-text-italic.svg \
    MetroMumble/actions/format-text-underline.svg \
    MetroMumble/actions/media-record.svg \
    MetroMumble/categories/applications-internet.svg \
    MetroMumble/controls/arrow_down.svg \
    MetroMumble/controls/arrow_down_disabled.svg \
    MetroMumble/controls/arrow_up.svg \
    MetroMumble/controls/arrow_up_disabled.svg \
    MetroMumble/controls/branch_closed.svg \
    MetroMumble/controls/branch_open.svg \
    MetroMumble/controls/checkbox_check_dark.svg \
    MetroMumble/controls/checkbox_check_disabled.svg \
    MetroMumble/controls/checkbox_check_lite.svg \
    MetroMumble/controls/handle_horizontal.svg \
    MetroMumble/controls/handle_vertical.svg \
    MetroMumble/controls/radio_check_dark.svg \
    MetroMumble/controls/radio_check_disabled.svg \
    MetroMumble/controls/radio_check_lite.svg \
    MetroMumble/controls/toolbar_ext.svg \
    MetroMumble/emblems/emblem-favorite.svg \
    MetroMumble/exported/config_asio.svg \
    MetroMumble/exported/config_audio_input.svg \
    MetroMumble/exported/config_audio_output.svg \
    MetroMumble/exported/config_basic.svg \
    MetroMumble/exported/config_lcd.svg \
    MetroMumble/exported/config_msgs.svg \
    MetroMumble/exported/config_network.svg \
    MetroMumble/exported/config_osd.svg \
    MetroMumble/exported/config_plugin.svg \
    MetroMumble/exported/config_shortcuts.svg \
    MetroMumble/exported/config_ui.svg \
    MetroMumble/mimetypes/image-x-generic.svg \
    MetroMumble/mimetypes/text-html.svg \
    MetroMumble/status/text-missing.svg \
    MetroMumble/authenticated.svg \
    MetroMumble/channel.svg \
    MetroMumble/channel_active.svg \
    MetroMumble/channel_linked.svg \
    MetroMumble/comment.svg \
    MetroMumble/comment_seen.svg \
    MetroMumble/deafened_self.svg \
    MetroMumble/deafened_server.svg \
    MetroMumble/default_avatar.svg \
    MetroMumble/filter.svg \
    MetroMumble/filter_off.svg \
    MetroMumble/filter_on.svg \
    MetroMumble/Information_icon.svg \
    MetroMumble/layout_classic.svg \
    MetroMumble/layout_custom.svg \
    MetroMumble/layout_hybrid.svg \
    MetroMumble/layout_stacked.svg \
    MetroMumble/mumble.svg \
    MetroMumble/muted_local.svg \
    MetroMumble/muted_self.svg \
    MetroMumble/muted_server.svg \
    MetroMumble/muted_suppressed.svg \
    MetroMumble/priority_speaker.svg \
    MetroMumble/self_comment.svg \
    MetroMumble/self_undeafened.svg \
    MetroMumble/talking_alt.svg \
    MetroMumble/talking_off.svg \
    MetroMumble/talking_on.svg \
    MetroMumble/talking_whisper.svg \
    MetroMumble/mumble.ico \
    icon.icns \
    qss/blue.css \
    qss/brown.css \
    qss/dev.css \
    qss/lightgray.css \
    qss/MetroDark.css \
    qss/silvery.css \
    MetroMumble/OSX/OSX Dark.qss \
    MetroMumble/OSX/OSX Lite.qss \
    MetroMumble/osx source/OSX Dark.scss \
    MetroMumble/osx source/OSX Lite.scss \
    MetroMumble/source/MetroBase.scss \
    MetroMumble/source/MetroDark.scss \
    MetroMumble/source/MetroLite.scss \
    MetroMumble/LICENSE \
    MetroMumble/MetroLite.qss \
    MetroMumble/overlay.mumblelay \
    MetroMumble/theme.ini \
    MetroMumble/README.md
