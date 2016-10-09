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

#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/export.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/list.hpp>

#include <sstream>
#include <fstream>
#include <glob.h>

#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL

using namespace seeta;
using namespace std;

void saveFeaturesFilePair(std::pair<vector<string>, vector<vector<float> >>  &features, string &filename){
    ofstream out(filename.c_str());
    stringstream ss;
    boost::archive::binary_oarchive oa(ss);
    oa << features.first << features.second;
    out << ss.str();
    out.close();
}

void loadFeaturesFilePair(std::pair<vector<string>, vector<vector<float> >> &features, string &filename){
    ifstream in(filename.c_str());
    boost::archive::binary_iarchive ia(in);
    ia >> features.first >> features.second;
    in.close();
}

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

std::vector<std::string> globVector(const std::string& pattern){
    glob_t glob_result;
    glob(pattern.c_str(),GLOB_TILDE,NULL,&glob_result);
    std::vector<std::string> files;
    for(unsigned int i=0;i<glob_result.gl_pathc;++i){
        files.push_back(std::string(glob_result.gl_pathv[i]));
    }
    globfree(&glob_result);
    std::sort(files.begin(),files.end());
    return files;
}


int main(int argc, char* argv[]) {
    
    // Initialize face detection model
    seeta::FaceDetection detector("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fd_frontal_v1.0.bin");
    detector.SetMinFaceSize(20);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);
    
    std::pair<vector<string>, vector<vector<float> >>  namesFeats;
    vector<vector<float> > feats;
    
    // Initialize face alignment model
    seeta::FaceAlignment point_detector("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fa_v1.1.bin");
    // Initialize face Identification model
    FaceIdentification face_recognizer("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fr_v1.0.bin");
    

    cv::Mat dst_img(face_recognizer.crop_height(), face_recognizer.crop_width(), CV_8UC(face_recognizer.crop_channels()));
    
    vector<string> imgNames = globVector("/Users/willard/codes/cpp/face/SeetaFaceLib/data/test_face_recognizer/images/src/*");
    
    for(auto itemImg:imgNames){
        float feat[2048];
        
        cv::Mat img_color = cv::imread(itemImg);
        extractFeat(detector, point_detector, face_recognizer, img_color, dst_img, feat);
        
        std::vector<float> featVector(std::begin(feat), std::end(feat));
        feats.push_back(featVector);
        
        //Show crop face
        cv::imshow("Original Face", img_color);
        cv::imshow("Crop Face", dst_img);
        cv::waitKey(10);
    }
    
    namesFeats.first = imgNames;
    namesFeats.second = feats;
    
    return 0;
}
