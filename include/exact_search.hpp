#pragma once

#include <string>
#include <vector>
#include "database.hpp"

namespace image_search{

    class ExactSearch {
    public:


    private:
        //Synartisi gia ton ypologismo ths eykleidias apostasis
        static float squareDistance(const std::vector<float>& a,const std::vector<float>& b);
    };

}