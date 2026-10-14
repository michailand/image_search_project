#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>


namespace image_search{
class BoVW {
public:
    bool buildVocabulary(const std::string& hpatchesDir,const std::vector<std::string>& trainSequences,int KClusters,int maxDescriptorPerImage=500,unsigned int seed=1,int maxIters=30);


private:
    //Πίνακας στον οποίο θα αποθηκεύσουμε το τελικό λεξικό μας
    cv::Mat vocabulary_;
};



}