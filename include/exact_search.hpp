#pragma once

#include <string>
#include <vector>
#include "database.hpp"

namespace image_search{

    //Δομη στην οποια θα κραταμε το αποτελεσμα της αναζητησης δηλαδη θα κραταμε το path της εικονας και την αποσταση της απο το query
    struct SearchResult{
        std::string id;
        float distance;
    };

    class ExactSearch {
    public:

        //ο constructor θα παιρνει σαν ορισμα την βαση δεδομενων
        ExactSearch(const Database& db);

        //Synarthsh exantlitikis anazitisis
        std::vector<SearchResult> search(const std::vector<float>& queryVec, int topK=10) const;

    private:
        //Synartisi gia ton ypologismo ths eykleidias apostasis
        static float squareDistance(const std::vector<float>& a,const std::vector<float>& b);

        const Database& db_;
    };

}