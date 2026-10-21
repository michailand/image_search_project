#pragma once

#include <vector>
#include <string>
#include <map>
#include "database.hpp"
#include "kmeans.hpp"
#include "exact_search.hpp"



namespace image_search {

class IVFFlat {
    public:


    private:
        //Synartisi gia ton ypologismo ths eykleidias apostasis
        static float squareDistance(const std::vector<float>& a,const std::vector<float>& b);
        
};

}