#include "kmeans.hpp"
#include <iostream>
#include <random>
#include <numeric>
#include <limits>
#include <algorithm>
#include <cstdlib>

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

void KMeans::initializeCentroids(const cv::Mat& data){
    //κρατάμε από τον πίνακα data ο οποίος έχει όλα τα descriptors, το πλήθος αυτών (n) καθώς και τις διαστάσεις (dims)
    int n= data.rows;
    int dims= data.cols;

    //Αρχικοποιούμε των πίνακα με τα κέντρα του visual vocabulary. Το πλήθος των κεντρων δίνεται αυθαίρετα
    centroids_=cv::Mat(k_,dims,CV_32F);

    //θα φτιάξω έναν πίνακα μεγέθους n με σκοπο να τον ανακατέψω και να διαλέξω τυχαία k κέντρα
    std::vector<int> tmp(n);
    std::iota(tmp.begin(),tmp.end(),0);// αρχικοποίηση 0....n-1

    std::mt19937 rng(seed_);
    std::shuffle(tmp.begin(),tmp.end(),rng);

    //Τωρα που ανακάτεψα των πίνακα θα διαλεξω τα Κ πρωτα στοιχεια του και οτι αριθμό έχουν θα τον χρησιμοποιήσω για δεικτη στον πίνακα Mat data
    if(n>=k_){
        for(int i=0;i<k_;i++){
            data.row(tmp[i]).copyTo(centroids_.row(i));
        }
    }
    else{
        std::cerr << "Τα κέντρα που επέλεξες για την αρχικοποίηση kmeans ειναι περισσότερα από τα SIFT descriptors. "<< n << " Δωσε πλήθος κ_ <= " << n << "\n";
        std::exit(EXIT_FAILURE);
    }
}

}