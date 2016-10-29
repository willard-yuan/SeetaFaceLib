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

// Get a file name from a path
std::string base_name(std::string const & path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}

int main(int argc, char* argv[]) {
    
    // Initialize face detection model
    seeta::FaceDetection detector("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fd_frontal_v1.0.bin");
    detector.SetMinFaceSize(20);
    detector.SetScoreThresh(2.f);
    detector.SetImagePyramidScaleFactor(0.8f);
    detector.SetWindowStep(4, 4);
    
    std::pair<vector<string>, vector<vector<float> >>  namesFeats;
    
    // Initialize face alignment model and face Identification model
    seeta::FaceAlignment point_detector("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fa_v1.1.bin");
    FaceIdentification face_recognizer("/Users/willard/codes/cpp/face/SeetaFaceLib/model/seeta_fr_v1.0.bin");
    
    cv::Mat dst_img(face_recognizer.crop_height(), face_recognizer.crop_width(), CV_8UC(face_recognizer.crop_channels()));
    
    // Save Cropped faces
    string path_imgCroppedNames = "/Users/willard/Pictures/SeetaFaces/";
    
    // Load image names and features
    string filenamePair = "namesFeats.bin";
    loadFeaturesFilePair(namesFeats, filenamePair);

    // Extract query face image feature
    float queryFeat[2048];
    cv::Mat queryImg_color = cv::imread("/Users/willard/Pictures/SeetaFaces/NF_200001_001.jpg");
    extractFeat(detector, point_detector, face_recognizer, queryImg_color, dst_img, queryFeat);
    
    // Calculate cosine distance between query and data base faces
    vector<pair<float,size_t> > dists_idxs;
    int i = 0;
    for(auto featItem: namesFeats.second){
        // http://stackoverflow.com/questions/2923272/how-to-convert-vector-to-array-c
        float tmp_cosine_dist = face_recognizer.CalcSimilarity(&namesFeats.second[0][0], &featItem[0]);
        dists_idxs.push_back(std::make_pair(tmp_cosine_dist, i++));
    }
    
    // Sorting will put lower values ahead of larger ones, resolving ties using the original index
    std::sort(dists_idxs.begin(), dists_idxs.end());
    std::reverse(dists_idxs.begin(), dists_idxs.end());
    for (size_t i = 0 ; i != dists_idxs.size() ; i++) {
        printf("distance: %f, face image: %s\n", dists_idxs[i].first, namesFeats.first.at(dists_idxs[i].second).c_str());
    }
    
    return 0;
}
