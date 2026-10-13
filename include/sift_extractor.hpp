#pragma once

#include <string>
#include <opencv2/opencv.hpp>

namespace image_search{

    //Εξάγει SIFT descriptors από μια εικόνα.
    //Αν maxDescriptors >0 και βρεθούν περισσότεροι από όσους έχεις ορισει τότε διαλέγει τυχαία τα maxDescriptors
    cv::Mat extractSIFTDescriptors(const std::string& imagePath, int maxDescriptors=0, unsigned int seed=42);
    
}