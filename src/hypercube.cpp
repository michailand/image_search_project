#include "hypercube.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <queue>

namespace image_search {

float HypercubeSearch::squaredDistance(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0.0f;
    for (size_t i=0;i<a.size();i++) {
        float diff =a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}

HypercubeSearch::HypercubeSearch(const Database& db, int kproj, float w, int M, int probes, int seed): db_(db),kproj_(kproj),w_(w),M_(M),probes_(probes) {

    const auto& items= db_.getItems();
    if(items.empty()){
        std::cout<< "ΠΡΟΣΟΧΗ!!! Το πλήθος εικόνων της βασης ειναι ΜΗΔΕΝΙΚΟ\n";
        return;
    }

    size_t dim= items[0].vec.size(); // Διαστασεις BoVW διανυσματων


    //Αρχικοποιηση της γεννητριας τυχαιων αριθμων. Σκοπος μας ειναι καθε φορα που αρχικοποιουμε να εχουμε ίδια αποτελεσματα  
    std::mt19937 gen(seed);
    std::normal_distribution<float> d_normal(0.0f, 1.0f);
    std::uniform_real_distribution<float> d_uniform(0.0f, w_);
    std::uniform_int_distribution<uint32_t> d_salt(1u, 0xFFFFFFFFu);
}

}