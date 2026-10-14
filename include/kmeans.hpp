#pragma once

#include <opencv2/opencv.hpp>
#include <vector>


namespace image_search {

class KMeans {



private:

    int k_;// πλήθος κέντρων για αρχικοποίηση kmeans
    cv::Mat centroids_;// Ο πίνακας με τα τελικά κέντρα του visual vocabulary
    unsigned int seed_;// Το seed που θα δώσουμε για την τυχαία αρχικόποιηση του kmeans

    //Αρχικοποίηση των Κ κέντρων με τυχαία επιλογή από τα δεδομένα μας με βάση το seed
    void initializeCentroids(const cv::Mat& data);

    //βοηθητική συνάρτηση για υπολογισμό ευκλείδιας αποστάσης μεταξύ δύο διανυσμάτων με πλήθος ορισμάτων dims
    static float squaredDistance(const float* a,const float* b, int dims);

    
};


}