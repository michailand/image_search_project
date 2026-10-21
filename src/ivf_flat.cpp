#include "ivf_flat.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>




namespace image_search{

    float IVFFlat::squaredDistance(const std::vector<float>& a,const std::vector<float>& b){
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

        //Τωρα θα εκτελεσουμε KMeans για kclusters κέντρα στον πινακα Mat data.
        // Αρα ετσι θα εχουμε τα νεα κεντρα για το λεξικο μας
        std::cout<< "Εναρξη δημιουργιας kmeans στη βαση δεδομενων με " << kclusters_ << " συστάδες\n";
        KMeans kmeans(kclusters_, 30, seed);
        centroids_= kmeans.fit(data);


        //Αφου εχουμε τα νεα κεντρα του λεξικου θα ψαξουμε να βρουμε για καθε εικονα του λεξικου ποιο κεντρο εχει ποιο κοντινο της και θα την ενταξουμε σε αυτο
        std::cout<< "Εναρξη ενταξης των εικονων του λεξικου στις " << kclusters_ << " συστάδες\n";
        for(int i=0; i<n; i++){// Gia oles tis eikones toy lexikoy( database db_) ypologise thn apostasi apo kathe centroids_
            
            //Βοηθητικες τιμες για να βρουμε το κοντινοτερο κεντρο για το items[i] δηλαδη για την καθ εικονα της βασης δεδομενων
            int bestCluster=-1;
            float minDist= std::numeric_limits<float>::max();

            for(int c=0;c<kclusters_; c++){
                //θα μετατρεψω παλι τον πίνακα centroids_ απο Ματ σε βεκτορ για να μπορεσω να υπολογισω τις ευκλειδιες αποστασεις
                std::vector<float> centroidVec(K);
                for (int j=0;j<K; j++){
                    centroidVec[j]= centroids_.at<float>(c,j);
                }

                float dist= squaredDistance(items[i].vec,centroidVec);
                if(dist < minDist){
                    minDist=dist;
                    bestCluster=c;
                }

            }

            //αφου βρηκαμε ποιο ειναι το κοντινοτερο κέντρο του το προσθετουμε στο vector του αντιστοιχου κεντρου στο map index_
            index_[bestCluster].push_back(items[i]);
        }

        std::cout << "[IVFFlat] To Inverted Index είναι έτοιμο. Τελος ενταξης των εικονων του λεξικου στις " << kclusters_ << " συστάδες\n";
    }


    

}