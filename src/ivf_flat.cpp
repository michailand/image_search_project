#include "ivf_flat.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>




namespace search_image{

    float ExactSearch::squareDistance(const std::vector<float>& a,const std::vector<float>& b){
        float sum= 0.0f;
        for(size_t i=0;i<a.size();i++){
            float diff= a[i]-b[i];
            sum+=diff* diff;
        }
        return sum;
    }


}