#include "sift_extractor.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <numeric>


namespace image_search {

    cv::Mat extractSIFTDescriptors(const std::string& imagePath, int maxDescriptors, unsigned int seed){

        //Ξεκινάμε το διάβασμα της εικόνας σε κλίμακα του γκρι
        cv::Mat image=cv::imread(imagePath, cv::IMREAD_GRAYSCALE);

        //Περίπτωση που δεν καταφερε να διαβασει την εικόνα
        if(image.empty()){
            std::cerr << "Σφάλμα κατα το άνοιγμα της εικόνας για την δημιουργία SIFT descriptors \n";
            return cv::Mat();
        }

        //
    }
}