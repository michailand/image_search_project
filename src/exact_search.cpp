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
        std::vector<SearchResult> results;
        const auto& items= db_.getItems();


        results.reserve(items.size());

        //Ypologismos ths apostasths apo to query me kathe stoixeio ths vasis
        for(const auto& item: items){
            float dist= squareDistance(queryVec, item.vec);
            results.push_back({item.id, dist});
        }

        //taxinomo gia na vro ta topK kontina stoixeia
        int k= std::min(topK,static_cast<int>(results.size()));
        std::partial_sort(results.begin(), results.begin() + k, results.end(),[](const SearchResult& a, const SearchResult& b) {
            return a.distance < b.distance;
        });

    //Κρατάμε μόνο τα πρώτα K
    results.resize(k);
    return results;
    }


}