#include <iostream>
#include "../myLibrary/myImageSequence/myImageSequence.h"
#include "../myLibrary/myFeatureExtractor/myFeatureExtractor.h"
#include <opencv2/core.hpp>
#include <opencv2/ml.hpp>

int main(void) {
    using namespace std;

    const cv::Size2i BlockSize(8, 8);
    const string sRootPath = "D:/Dataset/01/";
    const bool bHOG = true;
    const bool bLBP = false;

    //  read the smv model
    cv::Ptr<cv::ml::SVM> poSVM = cv::ml::StatModel::load<cv::ml::SVM>("hog_02.xml");
    
    // read the positive smaples and calculate the hog feature
    myImageSequence oPositiveReader(sRootPath + "Positive/", "", "bmp", false);
    oPositiveReader.SetAttribute(myImageSequence::Attribute::PADDING_LENGTH, 6);
    std::cout << "loading positive images" << std::endl;
    cv::Mat mPositiveSample;
    while (oPositiveReader >> mPositiveSample) {
        std::cout << "\r" << oPositiveReader.GetSequenceNumberString();
        myFeatureExtractor oExtractor(mPositiveSample, BlockSize);
        if (bHOG) {
            oExtractor.EnableFeature(myFeatureExtractor::Features::HOG_WITHOUT_NORM);
        }
        if (bLBP) {
            oExtractor.EnableFeature(myFeatureExtractor::Features::LBP_8_1_UNIFORM);
        }
        vector<vector<float>> vvfHOGFeature;
        
        for (int y = 0; y < mPositiveSample.rows / BlockSize.height; ++y) {
            for (int x = 0; x < mPositiveSample.cols / BlockSize.width; ++x) {
                vector<float> vfFeature;
                oExtractor.Describe(cv::Point2i(x * BlockSize.width, y * BlockSize.height), vfFeature);
                vvfHOGFeature.push_back(vfFeature);
            }
        }

        cv::Mat mSample(1, static_cast<int>(vvfHOGFeature.size() * vvfHOGFeature.at(0).size()), CV_32FC1);
        int i = 0;
        for (const auto& vfHOGFeature : vvfHOGFeature) {
            for (const auto fFeature : vfHOGFeature) {
                mSample.at<float>(0, i++) = fFeature;
            }
        }

        auto result = static_cast<int>(poSVM->predict(mSample));
        if (result != 1) {
            std::string sPath = "Wrong/" + std::string("pos") + oPositiveReader.GetSequenceNumberString() + std::string(".jpg");
            cv::imwrite(sPath, mPositiveSample);
        }
    }
    
    //read the negative smaples and calculate the hog feature
    myImageSequence oNegativeReader(sRootPath + "Negative/", "", "bmp", false);
    oNegativeReader.SetAttribute(myImageSequence::Attribute::PADDING_LENGTH, 6);
    std::cout << std::endl << "loading negative images" << std::endl;
    cv::Mat mNegativeSample;
    vector<vector<float>> vvfNegativeFeatures;
    while (oNegativeReader >> mNegativeSample) {
        myFeatureExtractor oExtractor(mNegativeSample, BlockSize);
        if (bHOG) {
            oExtractor.EnableFeature(myFeatureExtractor::Features::HOG_WITHOUT_NORM);
        }
        if (bLBP) {
            oExtractor.EnableFeature(myFeatureExtractor::Features::LBP_8_1_UNIFORM);
        }

        vector<vector<float>> vvfHOGFeature;
        for (int y = 0; y < mNegativeSample.rows / BlockSize.height; ++y) {
            for (int x = 0; x < mNegativeSample.cols / BlockSize.width; ++x) {
                vector<float> vfFeature;
                oExtractor.Describe(cv::Point2i(x * BlockSize.width, y * BlockSize.height), vfFeature);
                vvfHOGFeature.push_back(vfFeature);
            }
        }

        cv::Mat mSample(1, static_cast<int>(vvfHOGFeature.size() * vvfHOGFeature.at(0).size()), CV_32FC1);
        int i = 0;
        for (const auto& vfHOGFeature : vvfHOGFeature) {
            for (const auto fFeature : vfHOGFeature) {
                mSample.at<float>(0, i++) = fFeature;
            }
        }

        auto result = static_cast<int>(poSVM->predict(mSample));
        if (result != -1) {
            std::string sPath = "Wrong/" + std::string("neg") + oNegativeReader.GetSequenceNumberString() + std::string(".jpg");
            cv::imwrite(sPath, mNegativeSample);
        }
    }
    
    return 0;
}