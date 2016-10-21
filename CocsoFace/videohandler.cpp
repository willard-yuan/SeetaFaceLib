#include "videohandler.h"

#include <QDebug>

VideoHandler::VideoHandler()
{
}

void VideoHandler::start(int cameraIdx)
{
    if(cameraIdx == (int)1 || cameraIdx == (int)0){
        cap.open(cameraIdx);
        qDebug() << "cap is opened...\n";
    }else{
        cv::VideoCapture cap1(testVideoName.toStdString());
        this->cap = cap1;
        //qDebug() << "Could not initialize capturing...\n";
        //return;
        //cap.open(1);
        qDebug() << "cap is opened...\n";
    }

    if(cap.isOpened())
    {
        cap.set(CV_CAP_PROP_FRAME_WIDTH, 320);
        cap.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
        cap.set(CV_CAP_PROP_FPS, 10);
        qDebug() << "Video information" <<
                ": width=" << cap.get(CV_CAP_PROP_FRAME_WIDTH) <<
                ", height=" << cap.get(CV_CAP_PROP_FRAME_HEIGHT) <<
                ", nframes=" << cap.get(CV_CAP_PROP_FRAME_COUNT) <<
                 ", frmRate=" << cap.get(CV_CAP_PROP_FPS) <<   endl;
    }else{
        qDebug() << "Could not initialize capturing...\n";
        return;
    }
}

void VideoHandler::stop()
{
    cap.release();
}

const Mat& VideoHandler::getFrame()
{
    cap >> frame;
    while(frame.data == NULL){
        //cv::imshow("test", frame);
        //cv::waitKey();
        cap >> frame;
    }
    // 调试使用
    //cv::imshow("test", frame);
    //cv::waitKey(1);
    return frame;
}
