#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "helper.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "face_identification.h"
#include "recognizer.h"

#include "math_functions.h"
#include "face_detection.h"
#include "face_alignment.h"

using namespace seeta;

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->initForm();
    // timer
    QObject::connect(&processTimer, SIGNAL(timeout()), this, SLOT(process()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ----------------------初始化主窗体---------------------------------------------
void MainWindow::initForm()
{
    // 设置皮肤
    QString qss;
    //QFile qssFile(":/qss/black.css");
    QFile qssFile(":/MetroMumble/MetroDark.qss");
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen()){
        qss = QLatin1String(qssFile.readAll());
        qApp->setStyleSheet(qss);
        qssFile.close();
    }

    setWindowIcon(QIcon(QStringLiteral(":icons/eyeMain.png")));

    mapTrackingMethods["椭圆法"] = QString("ellipseMethod");
    //mapTrackingMethods["重心法"] = QString("gravityMethod");

    // local status widget
    localStatus = new QLabel(this);
    localStatus->setHidden(true);
    ui->statusBar->addPermanentWidget(localStatus);

    // screen status widget
    screenStatus = new QLabel(this);
    screenStatus->setHidden(true);
    ui->statusBar->addPermanentWidget(screenStatus);

    this->MinFaceSize = 40; // 最小人脸尺寸
    ui->rMaxHorizontalSlider->setMinimum(20);
    ui->rMaxHorizontalSlider->setMaximum(110);
    ui->culrrentRadius->setText(QString::number(40));


    // Initialize face detection model
    seeta::FaceDetection detector("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fd_frontal_v1.0.bin");
    detector.SetMinFaceSize(this->MinFaceSize);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);

    // Initialize face alignment model
    seeta::FaceAlignment point_detector("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fa_v1.1.bin");

    // Initialize face Identification model
    FaceIdentification face_recognizer("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fr_v1.0.bin");

    // load image
    cv::Mat gallery_img_color = cv::imread("/Users/willard/codes/cpp/face/SeetaFaceLib/data/test_face_recognizer/compare_im/2.jpg", 1);
    cv::Mat gallery_img_gray;
    cv::cvtColor(gallery_img_color, gallery_img_gray, CV_BGR2GRAY);

    ImageData gallery_img_data_color(gallery_img_color.cols, gallery_img_color.rows, gallery_img_color.channels());
    gallery_img_data_color.data = gallery_img_color.data;
    ImageData gallery_img_data_gray(gallery_img_gray.cols, gallery_img_gray.rows, gallery_img_gray.channels());
    gallery_img_data_gray.data = gallery_img_gray.data;

    // Detect faces
    std::vector<seeta::FaceInfo> gallery_faces = detector.Detect(gallery_img_data_gray);
    int32_t gallery_face_num = static_cast<int32_t>(gallery_faces.size());

    if (gallery_face_num == 0){
        std::cout << "Faces are not detected.";
    }

    point_detector.PointDetectLandmarks(gallery_img_data_gray, gallery_faces[0], this->gallery_points);

    // Extract face identity feature
    face_recognizer.ExtractFeatureWithCrop(gallery_img_data_color, this->gallery_points, this->gallery_fea);
}


// -----------------------------render loop-----------------------------------------------
void MainWindow::process()
{
    Mat frame = videoHandler.getFrame();

    cv::Mat_<uchar> image;
    // Pick one channel if necessary, and crop it to get rid of borders
    if (frame.channels() == 1){
        image = frame;
    }else if (frame.channels() == 3){
        cv::cvtColor(frame, image, cv::COLOR_BGR2GRAY);
    }else{
        throw std::runtime_error("Unsupported number of channels");
    }

    cv::Point2f localPos;

    // Initialize face detection model
    seeta::FaceDetection detector("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fd_frontal_v1.0.bin");
    detector.SetMinFaceSize(this->MinFaceSize);
    //detector.SetMinFaceSize(40);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);

    cv::Mat probe_img_gray = image;
    ImageData probe_img_data_color(frame.cols, frame.rows, frame.channels());
    probe_img_data_color.data = frame.data;
    ImageData probe_img_data_gray(probe_img_gray.cols, probe_img_gray.rows, probe_img_gray.channels());
    probe_img_data_gray.data = probe_img_gray.data;
    std::vector<seeta::FaceInfo> probe_faces = detector.Detect(probe_img_data_gray);
    int32_t probe_face_num = static_cast<int32_t>(probe_faces.size());

    if (probe_face_num!=0)
    {
        // Initialize face alignment model
        seeta::FaceAlignment point_detector("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fa_v1.1.bin");

        // Initialize face Identification model
        FaceIdentification face_recognizer("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fr_v1.0.bin");

        point_detector.PointDetectLandmarks(probe_img_data_gray, probe_faces[0], this->probe_points);
        face_recognizer.ExtractFeatureWithCrop(probe_img_data_color, this->probe_points, this->probe_fea);

        for (int i = 0; i<5; i++){
            cv::circle(frame, cv::Point(probe_points[i].x, probe_points[i].y), 2,
                       CV_RGB(0, 255, 0), CV_FILLED);
        }

        // Caculate similarity of two faces
        float sim = face_recognizer.CalcSimilarity(this->gallery_fea, this->probe_fea);
        ui->similaritytextBrowser->setText(QString("%1").arg(sim));

        if(sim>0.62){
            ui->sameIDtextBrowser->setText("是");
        }else{
            ui->sameIDtextBrowser->setText("否");
        }

        // Visualize the results
        cv::rectangle(frame, cv::Point(probe_faces[0].bbox.x, probe_faces[0].bbox.y), cv::Point(probe_faces[0].bbox.x + probe_faces[0].bbox.width - 1, probe_faces[0].bbox.y + probe_faces[0].bbox.height - 1), CV_RGB(255, 0, 0), 2);
    }else{
        ui->similaritytextBrowser->setText("");
        ui->sameIDtextBrowser->setText("");
    }

    // set video
    ui->videoCanvas->setPixmap(QPixmap::fromImage(Helper::mat2qimage(frame)).scaled(640, 480));
    // draw status
    localStatus->setHidden(false);
    //localStatus->setText("眼睛坐标:"+calibrator.getPositionString(localPos, localPos));

    // screen positions
    Point2f screenPos;
    double relativeX;
    double relativeY;

}

// ----------------------------------------------------------------------------
// general slots
void MainWindow::exitClicked()
{
    qDebug("exit");
    QApplication::quit();
}

void MainWindow::aboutClicked()
{
    qDebug("about");
}

// ----------------------------------------------------------------------------
void MainWindow::on_startButton_toggled(bool checked)
{
    if (checked){
        // switching the video ON
        qDebug("video: ON");
        videoHandler.start(0);
        processTimer.start(PROCESS_TIMEOUT);
        ui->videoCanvas->setHidden(false);
        ui->calibrateButton->setEnabled(true);
        ui->recordButton->setEnabled(false);
    }else{
        if (ui->calibrateButton->isChecked())
        {
            ui->calibrateButton->click();
        }
        ui->calibrateButton->setEnabled(false);
        ui->recordButton->setEnabled(false);

        // switching the video OFF
        qDebug("video: OFF");
        videoHandler.stop(); //必须先释放视频
        processTimer.stop();
        ui->videoCanvas->setHidden(false);
        localStatus->setHidden(true);
    }
}

void MainWindow::on_rMaxHorizontalSlider_valueChanged(int value)
{
    this->MinFaceSize = ui->rMaxHorizontalSlider->value();
    //qDebug() << "rMax: " << this->Radius_Max;
    ui->culrrentRadius->setText(QString::number(this->MinFaceSize));
}


void MainWindow::on_testButton_toggled(bool checked)
{
    if(checked){
        QString testVideo = QFileDialog::getOpenFileName(this, "打开视频", QDir::currentPath(), "Document files (*.avi *.mp4);;All files(*.*)");
        videoHandler.testVideoName = testVideo;
        videoHandler.start(2);
        processTimer.start(PROCESS_TIMEOUT);
        ui->videoCanvas->setHidden(false);
    }else{
        videoHandler.stop(); //必须先释放视频
        processTimer.stop();
        ui->videoCanvas->setHidden(false);
        localStatus->setHidden(true);
    }
}
