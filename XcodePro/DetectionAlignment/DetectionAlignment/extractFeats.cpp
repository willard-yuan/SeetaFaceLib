#include "face_identification.h"
#include "recognizer.h"
#include "math_functions.h"
#include "face_detection.h"
#include "face_alignment.h"

#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL

using namespace seeta;
using namespace std;

bool extractFeat(seeta::FaceDetection &detector, seeta::FaceAlignment &point_detector, FaceIdentification &face_recognizer, cv::Mat &img_color, cv::Mat dst_img, float * feat){
    
    cv::Mat img_gray;
    cv::cvtColor(img_color, img_gray, CV_BGR2GRAY);
    
    ImageData img_data_color(img_color.cols, img_color.rows, img_color.channels());
    img_data_color.data = img_color.data;
    ImageData img_data_gray(img_gray.cols, img_gray.rows, img_gray.channels());
    img_data_gray.data = img_gray.data;
    
    // Detect faces
    std::vector<seeta::FaceInfo> img_faces = detector.Detect(img_data_gray);
    int32_t face_num = static_cast<int32_t>(img_faces.size());
    
    if (face_num == 0)
    {
        std::cout << "Faces are not detected.";
        return false;
    }
    
    // Detect 5 facial landmarks
    seeta::FacialLandmark five_points[5];
    point_detector.PointDetectLandmarks(img_data_gray, img_faces[0], five_points);
    
    // Create a image to store crop face.
    ImageData dst_img_data(dst_img.cols, dst_img.rows, dst_img.channels());
    dst_img_data.data = dst_img.data;
    /* Crop Face */
    face_recognizer.CropFace(img_data_color, five_points, dst_img_data);
    
    // Extract face identity feature
    face_recognizer.ExtractFeatureWithCrop(img_data_color, five_points, feat);
    
    return true;
}


int main(int argc, char* argv[]) {
    
    // Initialize face detection model
    seeta::FaceDetection detector("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fd_frontal_v1.0.bin");
    detector.SetMinFaceSize(20);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);
    
    // Initialize face alignment model
    seeta::FaceAlignment point_detector("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fa_v1.1.bin");
    // Initialize face Identification model
    FaceIdentification face_recognizer("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fr_v1.0.bin");
    
    float feat[2048];
    cv::Mat dst_img(face_recognizer.crop_height(), face_recognizer.crop_width(), CV_8UC(face_recognizer.crop_channels()));
    
    cv::Mat img_color = cv::imread("/Users/willard/codes/cpp/face/SeetaFaceLib/data/test_face_recognizer/compare_im/2.jpg", 1);
    extractFeat(detector, point_detector, face_recognizer, img_color, dst_img, feat);
    
    //Show crop face
    cv::imshow("Crop Face", dst_img);
    cv::waitKey(0);
    cv::destroyWindow("Crop Face");
    
    return 0;
}
