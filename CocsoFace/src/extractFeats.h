#ifndef EXTRACTFEATS_H
#define EXTRACTFEATS_H

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

using namespace seeta;
using namespace std;

class extractFeats
{
public:
    void saveFeaturesFilePair(std::pair<vector<string>, vector<vector<float> >>  &features, string &filename);
    void loadFeaturesFilePair(std::pair<vector<string>, vector<vector<float> >> &features, string &filename);
    bool extractFeat(seeta::FaceDetection *detector, seeta::FaceAlignment *point_detector,
                     FaceIdentification *face_recognizer, cv::Mat &img_color, cv::Mat dst_img, float * feat);
    std::vector<std::string> globVector(const std::string& pattern);
    std::string base_name(std::string const & path);

};

#endif // EXTRACTFEATS_H
