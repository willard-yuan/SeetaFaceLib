/*
 *
 * This file is part of the open-source SeetaFace engine, which includes three modules:
 * SeetaFace Detection, SeetaFace Alignment, and SeetaFace Identification.
 *
 * This file is an example of how to use SeetaFace engine for face detection, the
 * face detection method described in the following paper:
 *
 *
 *   Funnel-structured cascade for multi-view face detection with alignment awareness,
 *   Shuzhe Wu, Meina Kan, Zhenliang He, Shiguang Shan, Xilin Chen.
 *   In Neurocomputing (under review)
 *
 *
 * Copyright (C) 2016, Visual Information Processing and Learning (VIPL) group,
 * Institute of Computing Technology, Chinese Academy of Sciences, Beijing, China.
 *
 * The codes are mainly developed by Shuzhe Wu (a Ph.D supervised by Prof. Shiguang Shan)
 *
 * As an open-source face recognition engine: you can redistribute SeetaFace source codes
 * and/or modify it under the terms of the BSD 2-Clause License.
 *
 * You should have received a copy of the BSD 2-Clause License along with the software.
 * If not, see < https://opensource.org/licenses/BSD-2-Clause>.
 *
 * Contact Info: you can send an email to SeetaFace@vipl.ict.ac.cn for any problems.
 *
 * Note: the above information must be kept whenever or wherever the codes are used.
 *
 */

#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "face_detection.h"
#include "face_alignment.h"

int main(int argc, char** argv) {
    seeta::FaceDetection detector("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fd_frontal_v1.0.bin");
    
    // Detector setting
    detector.SetMinFaceSize(40);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);
    
    int pts_num = 5;
    
    // Initialize face alignment model
    seeta::FaceAlignment point_detector("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fa_v1.1.bin");
    
    // open camera
    cv::VideoCapture cap(0);
    if(!cap.isOpened()){
        std::cout << "Unable to open the camera\n";
        std::exit(-1);
    }
    
    cv::Mat img;
    
    int i = 0;
    while(true) {
        cap >> img;
        if(img.empty()) {
            std::cout << "Can't read frames from your camera\n";
            break;
        }
        
        std::cout << "frame:  " << ++i << std::endl;
        
        cv::Mat img_gray;
        
        if (img.channels() != 1)
            cv::cvtColor(img, img_gray, cv::COLOR_BGR2GRAY);
        else
            img_gray = img;
        
        seeta::ImageData img_data;
        img_data.data = img_gray.data;
        img_data.width = img_gray.cols;
        img_data.height = img_gray.rows;
        img_data.num_channels = 1;
        
        std::vector<seeta::FaceInfo> faces = detector.Detect(img_data);
        int32_t num_face = static_cast<int32_t>(faces.size());
        
        if (num_face == 0)
            continue;
        
        // Detect landmarks
        seeta::FacialLandmark points[5];
        point_detector.PointDetectLandmarks(img_data, faces[0], points);
        
        // Visualize the results
        cv::rectangle(img, cv::Point(faces[0].bbox.x, faces[0].bbox.y), cv::Point(faces[0].bbox.x + faces[0].bbox.width - 1, faces[0].bbox.y + faces[0].bbox.height - 1), CV_RGB(255, 0, 0), 2);
        
        for (int i = 0; i<pts_num; i++)
        {
            cv::circle(img, cv::Point(points[i].x, points[i].y), 4, CV_RGB(0, 255, 0), CV_FILLED);
        }
        
        cv::namedWindow("Test", cv::WINDOW_AUTOSIZE);
        cv::imshow("Test", img);
        cv::waitKey(1);
        
        if (cv::waitKey(10) != -1)
            break;
    }
    return 0;
}
