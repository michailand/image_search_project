#pragma once

#include <vector>
#include <string>
#include <map>
#include <random>
#include "database.hpp"
#include "exact_search.hpp"

namespace image_search {



class LSHSearch {
public:



private:

    static float squaredDistance(const std::vector<float>& a, const std::vector<float>& b);
};

}