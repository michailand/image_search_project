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

    // if(db_.size()>0){
    //     vectorDim_=db_.getVector(0).size();
    // }

}

}