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

    //Τωρα θα πρεπει να βρούμε τα coarse centers του database μας το οποιο περιέχεει τις εικονες σε μορφη BOVW 
    std::cout<< "Εναρξη εκπαίδευσης- ευρεσης Coarse centers (Συνολο coarse centers= "<< kclusters_<< ") του database με χρηση Kmeans\n";
    cv::Mat data(n,K,CV_32F);
    //Επειδή τα δεδομενα ειναι αποθηκευμενα σε μορφη DatabaseItem θα τα μετατρεψουμε σε Mat για να τα δουλεψει η kmeans.cpp
    for(int i=0;i<n;i++){
        for(int j=0;j<K;j++){
            data.at<float>(i,j)=items[i].vec[j];
        }
    }
    //τωρα που εχω μετατρεψει τις εικονες παλι σε μορφη Mat θα βρω τα centroids
    KMeans coarse_kmeans(kclusters_,30,seed);
    coarse_centroids_= coarse_kmeans.fit(data);
    
    //Υπολογισμος των residuals δηλαδη της αποστασης της καθε εικόνας απο τα coarse centroids
    std::cout<< "Εναρξη Υπολογισμού των residuals και τοποθετηση εικόνων στα clusters των coarse centers\n";
    std::vector<int> assignments(n);// Πίνακας ο οποιος αντιστοιχιζει εικονα-->coarse center που ανηκει
    cv::Mat residuals(n,K,CV_32F);
    //Για καθε εικονα (n)
    for(int i=0;i<n;i++){
        float minDist = std::numeric_limits<float>::max();
        int bestCluster = -1;
        //Για καθε centroid(kclusters_)
        for(int c=0;c<kclusters_;c++){
            //Μετατρεπω το Mat των centroids σε vector ωστε να μπορεσω να βρω την ευκλειδια αποσταση απο ολες τις εικονες του database μου
            std::vector<float> centroidVec(K);
            for (int j= 0;j< K; ++j){
                centroidVec[j] = coarse_centroids_.at<float>(c, j);
            }

            float dist=squaredDistance(items[i].vec, centroidVec);
            if(dist<minDist){// Βρισκω σε ποιο centroid-cluster ανήκει η τρεχουσα εικονα
                minDist=dist;
                bestCluster=c;
            }
        }
        assignments[i]=bestCluster;

        //Υπολογισμος του residual και αποθηκευση σε Mat 
        for(int j=0;j<K;j++){
            residuals.at<float>(i,j)=items[i].vec[j]- coarse_centroids_.at<float>(bestCluster,j);
        }

    }

    std::cout<<"Λήξη Υπολογισμού των residuals και ευρεσης των Coarse Centers\n";


    //Τωρα θα γίνει η έυρεση των κεντρων ( συνηθως 256) του καθε υπολεξικου (συνολο M_ υπολεξικα) με την χρηση του Kmeans
    std::cout<< "Εναρξη ευρεσης των κέντρων του κάθε υπολεξικού. Συνολο υπολεξικων (PQ):"<<M_ << " Συνολο κεντρων: " << num_centroids_per_subspace_ << "\n";
    sub_codebooks_.resize(M_);// Αρχικοποίηση του vector υπολεξικων (PQ)

    for(int m=0; m<M_; m++){
        cv::Mat sub_data(n,sub_dim_,CV_32F);// Πίνακας Mat  ο οποιος θα βοηθησει στον υπολογισμό των kmeans
        for(int i=0;i<n;i++){
            for(int j=0;j<sub_dim_;j++){
                sub_data.at<float>(i,j)= residuals.at<float>(i,m*sub_dim_+j ); // i,m*sub_dim_+j--> Pointer arithmetic
            }
        }

        KMeans sub_kmeans(num_centroids_per_subspace_,30,seed+m); // Αλλάζω seed για να αυξησω την τυχαιότητα
        //Αποθηκευω στην καθε γραμμη του  vector τα κέντρα του καθε υπολεξικου. Αρα αν εχω 10 υπολεξικα και 256 κεντρα θα εχω τελικα εναν πινακα 10*256   
        sub_codebooks_[m]=sub_kmeans.fit(sub_data);
    }

    std::cout<< "Λήξη ευρεσης των κέντρων του κάθε υπολεξικού.\n";

    //Τωρα θα παρουμε ολες τις εικονες και θα διαιρεσουμε το διανυσμα τους με το Μ. Το καθε
    //μικροτερο διανυσμα που θα προκυψει θα το ενταξουμε στο κοντινοτερο κεντρο του αντιστοιχου υπολεξικου και ετσι
    //θα εχουμε φτιαξει το Inverted Index
    std::cout << "[IVFPQ] Κωδικοποίηση (Συμπίεση) και χτίσιμο Inverted Index\n";
    //Για καθε εικονα
    for(int i=0;i<n;i++){

        //Για καθε υπολεξικο
        for(int m=0;m< M_;m++){
            //Παιρνω από την εικονα τα υποδιαστηματα μήκους sub_dim_ και πληθους M απο οτν πινακα MAt και τα μετατρεπω σε float και τα αποθηκευω σε ενα vector
            std::vector<float> sub_residual(sub_dim_);
            for(int j=0;j<sub_dim_;j++){
                sub_residual[j]=residuals.at<float>(i,m*sub_dim_+ j);    
            } 

            
        }
    }
}


}