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

#include "falconn/eigen_wrapper.h"
#include "falconn/lsh_nn_table.h"

#include <QDesktopWidget>
#include <QStringListModel>
#include <QListWidget>
#include <QMessageBox>

#include <ctime>

#include <src/extractFeats.h>

using namespace seeta;

using falconn::construct_table;
using falconn::DenseVector;
using falconn::DistanceFunction;
using falconn::LSHConstructionParameters;
using falconn::LSHFamily;
using falconn::LSHNearestNeighborTable;
using falconn::QueryStatistics;
using falconn::StorageHashTable;

MainWindow::MainWindow(QWidget *parent) :QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // start the windows at center
    QRect desktopRect = QApplication::desktop()->availableGeometry(this);
    QPoint center = desktopRect.center();
    move(center.x()-width()*0.5, center.y()-height()*0.5);

    //QTimer::singleShot(1000, this, SLOT(showFullScreen()));

    //this->showMaximized();

    ui->scrlArea->setContentsMargins(0, 0, 0, 0);
    ui->scrlArea->setAlignment(Qt::AlignCenter);
    ui->scrlArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrlArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->scrlArea->setFrameStyle(0);
    ui->scrlArea->setWidgetResizable(true);


    imgs_listeWidget = new  QListWidget;
    imgs_listeWidget->setViewMode(QListWidget::IconMode);
    imgs_listeWidget->setIconSize(QSize(105, 120));
    imgs_listeWidget->setResizeMode(QListWidget::Adjust);
    imgs_listeWidget->setStyleSheet("QListWidget::item { width: 100; height: 140; font-size: 8pt; border-width: 5px 1px 5px 1px; padding-left: 10px; };"
                                    "QListView::item::text { font-size: 8pt; border-style: dot-dash; border-width: 5px 1px 5px 1px; border-color: yellow; }");
    imgs_listeWidget->setMovement(QListView::Static);

    this->initForm();
    // timer
    QObject::connect(&processTimer, SIGNAL(timeout()), this, SLOT(process()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 初始化主窗体
void MainWindow::initForm()
{
    // 设置皮肤
    QString qss;
    QFile qssFile(":/qss/black.css");
    qssFile.open(QFile::ReadOnly);
    if(qssFile.isOpen()){
        qss = QLatin1String(qssFile.readAll());
        qApp->setStyleSheet(qss);
        qssFile.close();
    }

    setWindowIcon(QIcon(QStringLiteral(":icons/eyeMain.png")));

    // local status widget
    localStatus = new QLabel(this);
    localStatus->setHidden(true);
    ui->statusBar->addPermanentWidget(localStatus);

    // screen status widget
    screenStatus = new QLabel(this);
    screenStatus->setHidden(true);
    ui->statusBar->addPermanentWidget(screenStatus);

    this->MinFaceSize = 20; // 最小人脸尺寸

    ui->rMaxHorizontalSlider->setMinimum(20);
    ui->rMaxHorizontalSlider->setMaximum(110);
    ui->culrrentRadius->setText(QString::number(20));

    this->ImagePyramidScaleFactor = 10; // 采样率
    ui->ScaleFactorQSlider->setMinimum(1);
    ui->ScaleFactorQSlider->setMaximum(10);
    ui->culrrentScaleFactorQLabel->setText(QString::number(1.0));

    this->numKNN = 50;
    this->numReranking = 10;

    // Initialize face detection model
    face_detector = new seeta::FaceDetection("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fd_frontal_v1.0.bin");
    face_detector->SetMinFaceSize(this->MinFaceSize);
    face_detector->SetScoreThresh(2.f);
    face_detector->SetImagePyramidScaleFactor(this->ImagePyramidScaleFactor);
    face_detector->SetWindowStep(4, 4);

    // Initialize face alignment model
    point_detector = new seeta::FaceAlignment("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fa_v1.1.bin");

    // Initialize face Identification model
    face_recognizer = new seeta::FaceIdentification("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fr_v1.0.bin");

    // load image
    cv::Mat gallery_img_color = cv::imread("/Users/willard/codes/cpp/face/SeetaFaceLib/data/test_face_recognizer/compare_im/2.jpg", 1);
    cv::Mat gallery_img_gray;
    cv::cvtColor(gallery_img_color, gallery_img_gray, CV_BGR2GRAY);

    dst_img = cv::Mat((int)face_recognizer->crop_height(), (int)face_recognizer->crop_width(), CV_8UC((int)face_recognizer->crop_channels()));

    // Save Cropped faces and feats
    path_imgCroppedNames = "/Users/willard/Pictures/SeetaFaces/";
    path_namesFeats = "/Users/willard/namesFeats.bin";

    ImageData gallery_img_data_color(gallery_img_color.cols, gallery_img_color.rows, gallery_img_color.channels());
    gallery_img_data_color.data = gallery_img_color.data;
    ImageData gallery_img_data_gray(gallery_img_gray.cols, gallery_img_gray.rows, gallery_img_gray.channels());
    gallery_img_data_gray.data = gallery_img_gray.data;

    // Detect faces
    std::vector<seeta::FaceInfo> gallery_faces = face_detector->Detect(gallery_img_data_gray);
    int32_t gallery_face_num = static_cast<int32_t>(gallery_faces.size());

    if (gallery_face_num == 0){
        std::cout << "Faces are not detected.";
    }

    point_detector->PointDetectLandmarks(gallery_img_data_gray, gallery_faces[0], this->gallery_points);

    // Extract face identity feature
    face_recognizer->ExtractFeatureWithCrop(gallery_img_data_color, this->gallery_points, this->gallery_fea);
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

    cv::Mat probe_img_gray = image;
    ImageData probe_img_data_color(frame.cols, frame.rows, frame.channels());
    probe_img_data_color.data = frame.data;
    ImageData probe_img_data_gray(probe_img_gray.cols, probe_img_gray.rows, probe_img_gray.channels());

    probe_img_data_gray.data = probe_img_gray.data;
    std::vector<seeta::FaceInfo> probe_faces = face_detector->Detect(probe_img_data_gray);
    int32_t probe_face_num = static_cast<int32_t>(probe_faces.size());

    if (probe_face_num!=0)
    {
        point_detector->PointDetectLandmarks(probe_img_data_gray, probe_faces[0], this->five_points);
        face_recognizer->ExtractFeatureWithCrop(probe_img_data_color, this->five_points, this->probe_fea);


        // Create a image to store crop face.
        ImageData dst_img_data(dst_img.cols, dst_img.rows, dst_img.channels());
        dst_img_data.data = dst_img.data;
        /* Crop Face */
        face_recognizer->CropFace(probe_img_data_color, this->five_points, dst_img_data);

        for (int i = 0; i<5; i++){
            cv::circle(frame, cv::Point(five_points[i].x, five_points[i].y), 2,
                       CV_RGB(0, 255, 0), CV_FILLED);
        }

        // Caculate similarity of two faces
        float sim = face_recognizer->CalcSimilarity(this->gallery_fea, this->probe_fea);
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
    ui->videoCanvas->setPixmap(QPixmap::fromImage(Helper::mat2qimage(frame)).scaled(320, 240));
    ui->cropImgLabel->setPixmap(QPixmap::fromImage(Helper::mat2qimage(dst_img)).scaled(320, 240));

}

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
        ui->ImgsOpenButton->setEnabled(true);
    }else{
        if (ui->ImgsOpenButton->isChecked())
        {
            ui->ImgsOpenButton->click();
        }
        ui->ImgsOpenButton->setEnabled(true);

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


void MainWindow::on_ScaleFactorQSlider_valueChanged(int value)
{
    this->ImagePyramidScaleFactor = ui->ScaleFactorQSlider->value();
    ui->culrrentScaleFactorQLabel->setText(QString::number(0.1*this->ImagePyramidScaleFactor));

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

void MainWindow::on_ImgsOpenButton_clicked()
{
    dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if ( !dir.isNull() )
    {
        // qDebug() << dir;
        QDir export_folder(dir);
        export_folder.setNameFilters(QStringList()<<"*.jpg");
        imgNamesQString = export_folder.entryList();
        // for ( QStringList::Iterator it = imgNamesQString.begin(); it != imgNamesQString.end(); ++it )
        // qDebug() << "Processed command: " << *it;
        QStringListModel *model = new QStringListModel(this);
        // Populate our model
        model->setStringList(imgNamesQString);
        // Glue model and view together
        ui->listView->setModel(model);
    }else{
        QMessageBox::information(NULL, "Title", "请选择图库", QMessageBox::Yes);
    }

}

// 右键菜单添加搜索功能
void MainWindow::searchSimilarImgs()
{
    imgs_listeWidget->clear();

    cv::Mat img_color = cv::imread(imgNameSelected.toStdString());
    idxCandidate = do_LSH_search(img_color);

    for (size_t i = 0 ; i != idxCandidate.size() ; i++) {
        QString tmpImgName = dir + '/' + namesFeats.first.at(idxCandidate[i]).c_str();
        imgs_listeWidget->addItem(new QListWidgetItem(QIcon(tmpImgName), QString::fromStdString(namesFeats.first.at(idxCandidate[i]).c_str())));
    }
    ui->previewImg->setPixmap(QPixmap::fromImage(Helper::mat2qimage(img_color)).scaled(320, 240));
    ui->cropImgLabel->setPixmap(QPixmap::fromImage(Helper::mat2qimage(dst_img)).scaled(320, 240));

    ui->scrlArea->setWidget(imgs_listeWidget);

    idxCandidate.clear();
}

// todo: 右键菜单添加搜索功能
void MainWindow::deleteImg(){
    qDebug()<<"hello world";
}

// 创建动作
void MainWindow::createActions()
{
}

// listView右键菜单添加搜索功能
void MainWindow::provideContextMenu(const QPoint &pos)
{
    QAction *searchAction = new QAction(tr("搜索"), this);
    searchAction->setObjectName("搜索");
    connect(searchAction, SIGNAL(triggered()), this, SLOT(searchSimilarImgs()));

    QAction *deleteAction = new QAction(tr("删除"), this);
    deleteAction->setObjectName("删除");
    connect(deleteAction, SIGNAL(triggered()), this, SLOT(deleteImg()));

    // Create menu and insert some actions
    myMenu = new QMenu();
    myMenu->addAction(searchAction);
    myMenu->addAction(deleteAction);

    // Handle global position
    QPoint globalPos = ui->listView->mapToGlobal(pos);

    // Show context menu at handling position
    myMenu->exec(globalPos);
    myMenu->close();
}


// listView
void MainWindow::on_listView_clicked(const QModelIndex &index)
{
    imgNameSelected =  dir+'/'+imgNamesQString.at(index.row());
    QPixmap img(imgNameSelected);
    ui->previewImg->setPixmap(img.scaled(200, 200));

    QString imgNameCropped =  QString::fromStdString(path_imgCroppedNames) + imgNamesQString.at(index.row());
    QPixmap imgCropped(imgNameCropped);
    ui->cropImgLabel->setPixmap(imgCropped.scaled(200, 200));

    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(provideContextMenu(QPoint)));
}

// 提取特征
void MainWindow::on_faceDetectionButton_clicked(bool checked)
{
    if(checked){
        int img_idx = 0;
        for(auto imgNameQString:imgNamesQString){

            float feat[2048];
            cv::Mat img_color = cv::imread((dir + '/' + imgNameQString).toStdString());

            featExtractor->extractFeat(face_detector, point_detector, face_recognizer, img_color, dst_img, feat);

            std::vector<float> featVector(std::begin(feat), std::end(feat));
            feats.push_back(featVector);

            imgNames.push_back(imgNameQString.toStdString());
            ++img_idx;
            //qDebug()<<imgNameQString<< "extracted, "<<img_idx<<"/"<< (int)imgNamesQString.size()<<"face images";

            cv::imwrite(path_imgCroppedNames+imgNameQString.toStdString(), dst_img);

            ui->previewImg->setPixmap(QPixmap::fromImage(Helper::mat2qimage(img_color)).scaled(320, 240));
            ui->cropImgLabel->setPixmap(QPixmap::fromImage(Helper::mat2qimage(dst_img)).scaled(320, 240));

            // Show crop face
            //cv::imshow("Original Face", img_color);
            cv::imshow("Crop Face", dst_img);
            cv::waitKey(1);
        }

        namesFeats.first = imgNames;
        namesFeats.second = feats;

        // Save image names and features
        featExtractor->saveFeaturesFilePair(namesFeats, path_namesFeats);
    }
}

// 搜索模块
std::vector<int32_t> MainWindow::do_LSH_search(cv::Mat &img_color){
    // Calculate cosine distance between query and data base faces
    float query_feat[2048];
    featExtractor->extractFeat(face_detector, point_detector, face_recognizer, img_color, dst_img, query_feat);

    falconn::DenseVector<float> q = Eigen::VectorXf::Map(&query_feat[0], 2048);
    q.normalize();

    if(namesFeats.first.empty()){
        featExtractor->loadFeaturesFilePair(namesFeats, path_namesFeats);
        qDebug()<<"first loaded";

        // LSH搜索方案
        int numFeats = (int)namesFeats.first.size();
        int dim = (int)namesFeats.second[0].size();

        // Data set parameters
        uint64_t seed = 119417657;

        // Common LSH parameters
        int num_tables = 8;
        int num_setup_threads = 0;
        StorageHashTable storage_hash_table = StorageHashTable::FlatHashTable;
        DistanceFunction distance_function = DistanceFunction::NegativeInnerProduct;

        // 转换数据类型
        qDebug() << "Generating data set ...";
        for (int ii = 0; ii < numFeats; ++ii) {
            falconn::DenseVector<float> v = Eigen::VectorXf::Map(&namesFeats.second[ii][0], dim);
            v.normalize(); // L2归一化
            data.push_back(v);
        }

        // Cross polytope hashing
        params_cp.dimension = dim;
        params_cp.lsh_family = LSHFamily::CrossPolytope;
        params_cp.distance_function = distance_function;
        params_cp.storage_hash_table = storage_hash_table;
        params_cp.k = 2; // 每个哈希表的哈希函数数目
        params_cp.l = num_tables; // 哈希表数目
        params_cp.last_cp_dimension = 2;
        params_cp.num_rotations = 2;
        params_cp.num_setup_threads = num_setup_threads;
        params_cp.seed = seed ^ 833840234;
        cptable = unique_ptr<falconn::LSHNearestNeighborTable<falconn::DenseVector<float>>>(std::move(construct_table<falconn::DenseVector<float>>(data, params_cp)));
        cptable->set_num_probes(896);
        qDebug() << "index build finished ...";

    }

    cptable->find_k_nearest_neighbors(q, numKNN, &idxCandidate); // LSH find the K nearest neighbors

    // do reranking
    std::vector<std::pair<float, size_t> > dists_idxs;
    for (int i = 0 ; i < numReranking ; i++) {
        float tmp_cosine_dist = q.dot(data[idxCandidate[i]]);
        dists_idxs.push_back(std::make_pair(tmp_cosine_dist, idxCandidate[i]));
    }

    std::sort(dists_idxs.begin(), dists_idxs.end());
    std::reverse(dists_idxs.begin(), dists_idxs.end());

    for(int i = 0 ; i < numReranking ; i++){
        idxCandidate.at(i) = (int32_t)dists_idxs[i].second;
    }
    return idxCandidate;
}


void MainWindow::on_queryButton_clicked()
{
    idxCandidate.clear();
    imgs_listeWidget->clear();

    QString path_queryImg = QFileDialog::getOpenFileName(this, "选择图像", QDir::currentPath(), "Document files (*.jpg *.png);;All files(*.*)");

    if ( !path_queryImg.isNull() )
    {
        cv::Mat img_color = cv::imread(path_queryImg.toStdString());
        idxCandidate = do_LSH_search(img_color);

        for (size_t i = 0 ; i != idxCandidate.size() ; i++) {
            QString tmpImgName = dir + '/' + namesFeats.first.at(idxCandidate[i]).c_str();
            imgs_listeWidget->addItem(new QListWidgetItem(QIcon(tmpImgName), QString::fromStdString(namesFeats.first.at(idxCandidate[i]).c_str())));
        }

        // 暴力搜索方案
        //        // Calculate cosine distance between query and data base faces
        //        clock_t begin = clock();
        //        std::vector<std::pair<float, size_t> > dists_idxs;
        //        int i = 0;
        //        for(auto featItem: namesFeats.second){
        //            // http://stackoverflow.com/questions/2923272/how-to-convert-vector-to-array-c
        //            float tmp_cosine_dist = face_recognizer->CalcSimilarity(query_feat, &featItem[0]);
        //            dists_idxs.push_back(std::make_pair(tmp_cosine_dist, i++));
        //        }
        //        std::sort(dists_idxs.begin(), dists_idxs.end());
        //        std::reverse(dists_idxs.begin(), dists_idxs.end());
        //        clock_t end = clock();
        //        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        //        qDebug()<<"elapsed_secs: " << elapsed_secs;

        //        for (size_t i = 0 ; i != dists_idxs.size() ; i++) {
        //            //qDebug()<<dists_idxs[i].first<<namesFeats.first.at(dists_idxs[i].second).c_str();
        //            QString tmpImgName = dir + '/' + namesFeats.first.at(dists_idxs[i].second).c_str();
        //            imgs_listeWidget->addItem(new QListWidgetItem(QIcon(tmpImgName), QString::fromStdString(namesFeats.first.at(dists_idxs[i].second).c_str())));
        //        }

        ui->previewImg->setPixmap(QPixmap::fromImage(Helper::mat2qimage(img_color)).scaled(320, 240));
        ui->cropImgLabel->setPixmap(QPixmap::fromImage(Helper::mat2qimage(dst_img)).scaled(320, 240));
        ui->scrlArea->setWidget(imgs_listeWidget);
    }
    else{
        QMessageBox::information(NULL, "Title", "请选择一张图片", QMessageBox::Yes);
    }
}
