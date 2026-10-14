#include "kmeans.hpp"
#include <iostream>
#include <random>
#include <numeric>
#include <limits>


namespace image_search{
//Παιρνει ως ορίσματα δυο διανυσματα (a & b) (vector) και το μέγεθος τους (dims) δηλαδή τις διαστάσεις του και επιστρέφει την απόσταση μεταξύ τους 
float KMeans::squaredDistance(const float* a,const float* b, int dims){
    float res=0.0f;
    for(int i=0;i<dims;i++){
        float tmp= a[i]-b[i];
        tmp= tmp *tmp;
        res+=tmp;
    }
    return res;
}

void KMeans::intializeCentroids(const cv::Mat& data){
    //κρατάμε από τον πίνακα data ο οποίος έχει όλα τα descriptors, το πλήθος αυτών (n) καθώς και τις διαστάσεις (dims)
    int n= data.rows;
    int dims= data.cols;

    //Αρχικοποιούμε των πίνακα με τα κέντρα του visual vocabulary. Το πλήθος των κεντρων δίνεται αυθαίρετα
    centroids_=cv::Mat(k_,dims,CV_32F);
}

}