#ifndef VIDEOHANDLER_H
#define VIDEOHANDLER_H

#include <QtGui/QImage>
#include <QFileDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo/photo.hpp>

using namespace cv;

class VideoHandler
{
public:
    VideoHandler();

    void start(int cameraIdx);
    //template <typename T>
    //void start(T cameraIdx);
    void stop();
    const Mat &getFrame();
    QString testVideoName;

private:
    VideoCapture cap;
    Mat frame;
};

#endif // VIDEOHANDLER_H
