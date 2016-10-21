#include "extractFeats.h"

#define fopen_s(pFile,filename,mode) ((*(pFile))=fopen((filename),(mode)))==NULL

void extractFeats::saveFeaturesFilePair(std::pair<vector<string>, vector<vector<float> >>  &features, string &filename){
    ofstream out(filename.c_str());
    stringstream ss;
    boost::archive::binary_oarchive oa(ss);
    oa << features.first << features.second;
    out << ss.str();
    out.close();
}

void extractFeats::loadFeaturesFilePair(std::pair<vector<string>, vector<vector<float> >> &features, string &filename){
    ifstream in(filename.c_str());
    boost::archive::binary_iarchive ia(in);
    ia >> features.first >> features.second;
    in.close();
}

bool extractFeats::extractFeat(seeta::FaceDetection * detector, seeta::FaceAlignment * point_detector,
                 FaceIdentification * face_recognizer, cv::Mat &img_color, cv::Mat dst_img, float * feat){
    cv::Mat img_gray;
    cv::cvtColor(img_color, img_gray, CV_BGR2GRAY);

    ImageData img_data_color(img_color.cols, img_color.rows, img_color.channels());
    img_data_color.data = img_color.data;
    ImageData img_data_gray(img_gray.cols, img_gray.rows, img_gray.channels());
    img_data_gray.data = img_gray.data;

    // Detect faces
    std::vector<seeta::FaceInfo> img_faces = detector->Detect(img_data_gray);
    int32_t face_num = static_cast<int32_t>(img_faces.size());

    if (face_num == 0)
    {
        std::cout << "Faces are not detected.";
        return false;
    }

    // Detect 5 facial landmarks
    seeta::FacialLandmark five_points[5];
    point_detector->PointDetectLandmarks(img_data_gray, img_faces[0], five_points);

    // Create a image to store crop face.
    ImageData dst_img_data(dst_img.cols, dst_img.rows, dst_img.channels());
    dst_img_data.data = dst_img.data;
    /* Crop Face */
    face_recognizer->CropFace(img_data_color, five_points, dst_img_data);

    // Extract face identity feature
    face_recognizer->ExtractFeatureWithCrop(img_data_color, five_points, feat);

    return true;
}

std::vector<std::string> extractFeats::globVector(const std::string& pattern){
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
std::string extractFeats::base_name(std::string const & path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}
