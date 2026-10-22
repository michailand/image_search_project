#pragma once

#include <vector>
#include <string>
#include <map>
#include <opencv2/opencv.hpp>
#include "database.hpp"
#include "exact_search.hpp"

namespace image_search {

class IVFPQ {
public:


private:

    static float squaredDistance(const std::vector<float>& a, const std::vector<float>& b);

};

}