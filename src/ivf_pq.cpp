#include "ivf_pq.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <limits>

namespace image_search {



float IVFPQ::squaredDistance(const std::vector<float>& a, const std::vector<float>& b) {
    float sum=0.0f;
    for (size_t i = 0; i< a.size();i++) {
        float diff= a[i]- b[i];
        sum +=diff* diff;
    }
    return sum;
}



IVFPQ::IVFPQ(const Database& db, int kclusters, int M, int nbits, int seed) : db_(db), kclusters_(kclusters), M_(M), nbits_(nbits) {

    const auto& items= db_.getItems();
    int n =items.size(); // πληθος των visual words

    //Αν δεν δοθουν ποσα coarse clusters θελουμε θα υπολοστει η τετραγωνικη ριζα συμφωνα με την εκφωνηση
    if(kclusters_<=0){
        kclusters_=static_cast<int>(std::ceil(std::sqrt(n)));
        std::cout << "[IVFPQ] Αυτόματος υπολογισμός kclusters: " << kclusters_ << "\n";
    }

    if(n==0){
        std::cout << "Η βαση δεδομενων των BoVW (Visual Words) δεν εχει οριστει σωστα γιατι τα Visual Words δεν εχουν διαστασεις \n" ;
        return;
    }

    //Θα κανουμε έλεγχο αν το residual μπορει να διαιρεθει ακριβώς με το Μ που δόθηκε. Αν οχι τοτε τερματισμός με ερρορ
    int K= items[0].size 

    

}
}