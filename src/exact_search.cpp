#include "exact_search.hpp"

namespace image_search{
    
    //παντα οταν δημιουργουμε το αντικειμενο θα του δινουμε προφανως και μια βαση δεδομενων
    ExactSearch::ExactSearch(const Database& db): db_(db){}

    float ExactSearch::squareDistance(const std::vector<float>& a,const std::vector<float>& b){
        float sum= 0.0f;
        for(size_t i=0;i<a.size();i++){
            float diff= a[i]-b[i];
            sum+=diff* diff;
        }
        return sum;
    }


    std::vector<SearchResult> ExactSearch::search(const std::vector<float>& queryVec, int topK) const {

    }


}