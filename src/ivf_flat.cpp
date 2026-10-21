#include "ivf_flat.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>




namespace image_search{

    float IVFFlat::squareDistance(const std::vector<float>& a,const std::vector<float>& b){
        float sum= 0.0f;
        for(size_t i=0;i<a.size();i++){
            float diff= a[i]-b[i];
            sum+=diff* diff;
        }
        return sum;
    }


    IVFFlat::IVFFlat(const Database& db, int kclusters, int seed) : db_(db), kclusters_(kclusters) {
        const auto& items= db_.getItems();
        int n=items.size(); // παιρνουμε το πλήθος εικόνων

        // Αν δεν δοθεί kclusters, χρησιμοποιούμε το προεπιλεγμένο της εκφώνησης: ceil(sqrt(n))
        if(kclusters_<=0){
            kclusters_=static_cast<int>(std::ceil(std::sqrt(n)));
            std::cout << "[IVFFlat] Αυτόματος υπολογισμός kclusters: "<< kclusters_ << "\n";
        }
    }

}