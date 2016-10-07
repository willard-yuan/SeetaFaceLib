#ifndef HELPER_H
#define HELPER_H

#include <QString>
#include <QDir>
#include <QImage>

#include <ctime>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

class Helper
{
public:
    Helper();

    static string timeToTimestamp(time_t t);
    static string timestampToFormatted(string t);

    static QImage mat2qimage(const Mat& mat);
    static Mat qimage2mat(const QImage& qimage);
};

#endif // HELPER_H
