#include "hypercube.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <queue>

namespace image_search {


HypercubeSearch::HypercubeSearch(const Database& db, int kproj, double w, int M, int probes, int seed): db_(db),kproj_(kproj),w_(w),M_(M),probes_(probes),seed_(seed) {

    if(db_.size()>0){
        vectorDim_=db_.getVector(0).size();
    }

}

}