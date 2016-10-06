#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QProgressBar>
#include <QTimer>
#include <QDebug>
#include "videohandler.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "face_identification.h"
#include "recognizer.h"
#include "math_functions.h"
#include "face_detection.h"
#include "face_alignment.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    static const int PROCESS_TIMEOUT = 50;

private:
    Ui::MainWindow *ui;
    QLabel* localStatus;
    QLabel* screenStatus;

    QTimer processTimer;

    VideoHandler videoHandler;
    void initForm();

    QMap<QString, QString> mapTrackingMethods;
    QString trackingMethod;
    int MinFaceSize;

    float gallery_fea[2048];
    float probe_fea[2048];
    seeta::FacialLandmark gallery_points[5];
    seeta::FacialLandmark probe_points[5];

public slots:
    // general
    void exitClicked();
    void aboutClicked();

private slots:
    void process();
    void on_startButton_toggled(bool checked);
    void on_rMaxHorizontalSlider_valueChanged(int value);
    void on_testButton_toggled(bool checked);
};

#endif // MAINWINDOW_H
