#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>



class BoVW {
public:
    bool buildVocabulary(const std::string& hpatchesDir,const std::vector<std::string>& trainSequences,)


private:
    //Πίνακας στον οποίο θα αποθηκεύσουμε το τελικό λεξικό μας
    cv::Mat vocabulary_;
};