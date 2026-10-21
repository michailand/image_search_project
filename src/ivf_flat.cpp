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

        if(n==0){
            std::cout<<"ΠΡΟΣΟΧΗ!!! Το πλήθος εικόνων της Database σου ειναι ΜΗΔΕΝΙΚΟ \n";
            return;
        }

        //Επειδή τα διανύσματα του Database δεν ειναι τυπου Mat θα πρεπει να τα μετατρεψουμε σε Μat πριν εφαρμοστει ο αλγοριθμος K-means
        int K= items[0].vec.size(); // Πληθος διαστάσεων του καθε διανυσματος του λεξικού. Συνήθως δίνουμε 100 αλλα δεν ειναι απαραιτητο
        cv::Mat data(n,K,CV_32F);
        for(int i=0;i<n;i++){
            for(int j=0;j< K;j++){
                data.at<float>(i,j)=items[i].vec[j];  // Δηλαδη παιρνει τους float απο το βεκτορ του databaseItem και τους αποθηκευει στο βεκτορ του map 
            }
        }
    }

}