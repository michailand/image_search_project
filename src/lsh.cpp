#include "lsh.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace image_search {

float LSHSearch::squaredDistance(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}


unsigned int LSHFamily::computeHash(const std::vector<float>& x) const {

}

}