#pragma once

#include <opencv2/opencv.hpp>
#include <vector>


namespace image_search {

class KMeans {
public:

    //Constructor:
    // k==πληθος κέντρων visual vocabulary
    // maxIters==πλήθος μέγιστου αριθμού επαναλήψεων για την οριστικοποίηση των k κέντρων
    // seed== το seed της γεννήτριας τυχαίων αριθμών
    KMeans(int k,int maxIters=30, unsigned int seed= 42);

    //Αλγόριθμος εκπαίδευσης για την δημιουργία k κέντρων
    cv::Mat fit(const cv::Mat& data);


private:

    int k_;// πλήθος κέντρων για αρχικοποίηση kmeans
    cv::Mat centroids_;// Ο πίνακας με τα τελικά κέντρα του visual vocabulary
    unsigned int seed_;// Το seed που θα δώσουμε για την τυχαία αρχικόποιηση του kmeans
    int maxIters_;//πλήθος μέγιστου αριθμού επαναλήψεων για την οριστικοποίηση των k κέντρων

    //Αρχικοποίηση των Κ κέντρων με τυχαία επιλογή από τα δεδομένα μας με βάση το seed
    void initializeCentroids(const cv::Mat& data);

    //βοηθητική συνάρτηση για υπολογισμό ευκλείδιας αποστάσης μεταξύ δύο διανυσμάτων με πλήθος ορισμάτων dims
    static float squaredDistance(const float* a,const float* b, int dims);

    
};


}