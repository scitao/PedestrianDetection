#include "myAdaBoost.h"

const double myAdaBoost::TRIM_RATE = 0.95;

myAdaBoost::myAdaBoost(unsigned int iWeakCount) {
    // create adaboost
    auto pBoost = cv::ml::Boost::create();
    m_poClassifier = pBoost;

    // set parameter
    pBoost->setBoostType(myAdaBoost::BOOST_TYPE);
    pBoost->setWeightTrimRate(myAdaBoost::TRIM_RATE);
    pBoost->setWeakCount(iWeakCount);
}

myAdaBoost::~myAdaBoost(void) {}