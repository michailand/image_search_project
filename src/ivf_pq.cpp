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
    int n =items.size(); // πληθος των εικόνων με διανυσμα διαστάσεων οσο και τα visual words

    //Αν δεν δοθουν ποσα coarse clusters θελουμε θα υπολοστει η τετραγωνικη ριζα συμφωνα με την εκφωνηση
    if(kclusters_<=0){
        kclusters_=static_cast<int>(std::ceil(std::sqrt(n)));
        std::cout << "[IVFPQ] Αυτόματος υπολογισμός kclusters: " << kclusters_ << "\n";
    }

    if(n==0){
        std::cout << "Η βαση δεδομενων των εικόνων BoVW είναι άδεια \n" ;
        return;
    }

    //K ειναι το μεγεθος του διανύσματος των λεξεων στην database 
    int K= items[0].vec.size();
    
    //Θα κανουμε έλεγχο αν το residual μπορει να διαιρεθει ακριβώς με το Μ που δόθηκε. Αν οχι τοτε τερματισμός με ερρορ
    if(K%M_ !=0 ){
        std::cerr<< "[IVFPQ] Σφάλμα:Η διάσταση K (" << K << ") πρέπει να διαιρείται ακριβώς με το M ("<< M_<< ").\n";
        exit(1);
    }    

    sub_dim_= K/ M_; // To μεγεθος που θα εχει το κάθε τμήμα του υπολεξικου
    num_centroids_per_subspace_=1<<nbits_; // 2^nbits (π.χ. 256)


    

}
}