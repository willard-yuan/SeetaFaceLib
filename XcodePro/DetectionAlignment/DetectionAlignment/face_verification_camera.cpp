#include<iostream>
using namespace std;

#ifdef _WIN32
#pragma once
#include <opencv2/core/version.hpp>

#define CV_VERSION_ID CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) \
CVAUX_STR(CV_SUBMINOR_VERSION)

#ifdef _DEBUG
#define cvLIB(name) "opencv_" name CV_VERSION_ID "d"
#else
#define cvLIB(name) "opencv_" name CV_VERSION_ID
#endif //_DEBUG

#pragma comment( lib, cvLIB("core") )
#pragma comment( lib, cvLIB("imgproc") )
#pragma comment( lib, cvLIB("highgui") )

#endif //_WIN32

#if defined(__unix__) || defined(__APPLE__)

#ifndef fopen_s

#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL

#endif //fopen_s

#endif //__unix

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include "face_identification.h"
#include "recognizer.h"

#include "math_functions.h"

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "face_detection.h"
#include "face_alignment.h"

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using namespace seeta;

#define TEST(major, minor) major##_##minor##_Tester()
#define EXPECT_NE(a, b) if ((a) == (b)) std::cout << "ERROR: "
#define EXPECT_EQ(a, b) if ((a) != (b)) std::cout << "ERROR: "


int main(int argc, char* argv[]) {
    // Initialize face detection model
    seeta::FaceDetection detector("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fd_frontal_v1.0.bin");
    detector.SetMinFaceSize(40);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);
    
    // Initialize face alignment model
    seeta::FaceAlignment point_detector("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fa_v1.1.bin");
    
    // Initialize face Identification model
    FaceIdentification face_recognizer("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fr_v1.0.bin");
    
    //load image
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
    
    if (gallery_face_num == 0)
    {
        std::cout << "Faces are not detected.";
        return 0;
    }
    
    // Detect 5 facial landmarks
    seeta::FacialLandmark gallery_points[5];
    point_detector.PointDetectLandmarks(gallery_img_data_gray, gallery_faces[0], gallery_points);
    
    // Extract face identity feature
    float gallery_fea[2048];
    face_recognizer.ExtractFeatureWithCrop(gallery_img_data_color, gallery_points, gallery_fea);
    
    // open camera
    cv::VideoCapture cap(0);
    if(!cap.isOpened()){
        std::cout << "Unable to open the camera\n";
        std::exit(-1);
    }
    
    cv::Mat probe_img_color;
    int i = 0;
    
    while(true) {
        
        float probe_fea[2048];
        seeta::FacialLandmark probe_points[5];
        
        cap >> probe_img_color;
        if(probe_img_color.empty()) {
            std::cout << "Can't read frames from your camera\n";
            break;
        }
        std::cout << "frame:  " << ++i << std::endl;
        
        cv::Mat probe_img_gray;
        
        if (probe_img_color.channels() != 1)
            cv::cvtColor(probe_img_color, probe_img_gray, cv::COLOR_BGR2GRAY);
        else
            probe_img_gray = probe_img_color;
        
        ImageData probe_img_data_color(probe_img_color.cols, probe_img_color.rows, probe_img_color.channels());
        probe_img_data_color.data = probe_img_color.data;
        
        ImageData probe_img_data_gray(probe_img_gray.cols, probe_img_gray.rows, probe_img_gray.channels());
        probe_img_data_gray.data = probe_img_gray.data;
        
        std::vector<seeta::FaceInfo> probe_faces = detector.Detect(probe_img_data_gray);
        int32_t probe_face_num = static_cast<int32_t>(probe_faces.size());
        
        if (probe_face_num!=0)
        {
            
            point_detector.PointDetectLandmarks(probe_img_data_gray, probe_faces[0], probe_points);
            face_recognizer.ExtractFeatureWithCrop(probe_img_data_color, probe_points, probe_fea);
            
            for (int i = 0; i<5; i++)
            {
                cv::circle(gallery_img_color, cv::Point(gallery_points[i].x, gallery_points[i].y), 2,
                           CV_RGB(0, 255, 0), CV_FILLED);
                cv::circle(probe_img_color, cv::Point(probe_points[i].x, probe_points[i].y), 2,
                           CV_RGB(0, 255, 0), CV_FILLED);
            }
            
            // Caculate similarity of two faces
            float sim = face_recognizer.CalcSimilarity(gallery_fea, probe_fea);
            std::cout << sim <<endl;
            
            cv::putText(probe_img_color, std::to_string(sim), cv::Point(100, 100), CV_FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(0,255,0), 2.0);
            if(sim>0.62)
                cv::putText(probe_img_color, "The same person", cv::Point(400, 100), CV_FONT_HERSHEY_SIMPLEX, 1.0, CV_RGB(255,0,0), 2.0);
        }
        
        cv::imshow("gallery_img_color", gallery_img_color);
        cv::imshow("probe_img_color", probe_img_color);
        
        cv::waitKey(1);
    }
    
    return 0;
}
