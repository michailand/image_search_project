#include "kmeans.hpp"
#include <iostream>
#include <random>
#include <numeric>
#include <limits>
#include <algorithm>
#include <cstdlib>

namespace image_search{
//Παιρνει ως ορίσματα δυο διανυσματα (a & b) (vector) και το μέγεθος τους (dims) δηλαδή τις διαστάσεις του και επιστρέφει την απόσταση μεταξύ τους 
float KMeans::squaredDistance(const float* a,const float* b, int dims){
    float res=0.0f;
    for(int i=0;i<dims;i++){
        float tmp= a[i]-b[i];
        tmp= tmp *tmp;
        res+=tmp;
    }
    return res;
}

void KMeans::initializeCentroids(const cv::Mat& data){
    //κρατάμε από τον πίνακα data ο οποίος έχει όλα τα descriptors, το πλήθος αυτών (n) καθώς και τις διαστάσεις (dims)
    int n= data.rows;
    int dims= data.cols;

    //Αρχικοποιούμε των πίνακα με τα κέντρα του visual vocabulary. Το πλήθος των κεντρων δίνεται αυθαίρετα
    centroids_=cv::Mat(k_,dims,CV_32F);

    //θα φτιάξω έναν πίνακα μεγέθους n με σκοπο να τον ανακατέψω και να διαλέξω τυχαία k κέντρα
    std::vector<int> tmp(n);
    std::iota(tmp.begin(),tmp.end(),0);// αρχικοποίηση 0....n-1

    std::mt19937 rng(seed_);
    std::shuffle(tmp.begin(),tmp.end(),rng);

    //Τωρα που ανακάτεψα των πίνακα θα διαλεξω τα Κ πρωτα στοιχεια του και οτι αριθμό έχουν θα τον χρησιμοποιήσω για δεικτη στον πίνακα Mat data
    if(n>=k_){
        for(int i=0;i<k_;i++){
            data.row(tmp[i]).copyTo(centroids_.row(i));
        }
    }
    else{
        std::cerr << "Τα κέντρα που επέλεξες για την αρχικοποίηση kmeans ειναι περισσότερα από τα SIFT descriptors. "<< n << " Δωσε πλήθος κ_ <= " << n << "\n";
        std::exit(EXIT_FAILURE);
    }
}


cv::Mat KMeans::fit(const cv::Mat& data){

    //Ελεγχος εισόδου δεδομένων
    if(data.empty() || data.rows<k_){
        std::cerr << "τα δεδομένα εισόδου τα οποία δόθηκαν για την δημιουργια kmeans ειναι είτε άδεια είτε λίγα σε πλήθος\n";
        return cv::Mat();
    }

    cv::Mat dataFloat;
    //Επειδή η opencv δουλευει με δεδομενα τυπου CV_32F πρεπει να το ελεγξουμε αλλιώς το κανουμε convert
    if(data.type()!=CV_32F){
        data.convertTo(dataFloat,CV_32F);
    }
    else{
        dataFloat=data;
    }

        
    int n=dataFloat.rows;
    int dims= dataFloat.cols;

    //Αρχικοπποίηση των κέντρων k (centroids)
    initializeCentroids(dataFloat);

    //Πίνακας μεγέθους n(πλήθος SIFT Descriptors) ο οποίος περιέχει για τον καθε  Descriptors το πλησιεστερο του Κεντρο
    std::vector<int> assignments(n,-1);
    std::mt19937 rng(seed_);

    std::cout<< "Ξεκινάμε το kmeans για κεντρα πλήθους Κ= "<< k_ << " και για Descriptors πληθους Point= "<<n<<"\n";

    //Υλοποίηση αλγορίθμου Lloyd
    //Θα τρεξουμε μεχρι maxIters_ φορές
    for (int iter = 0; iter < maxIters_; iter++) {
        int changedAssignments = 0;

        //Δομή δεδομένων στην οποία θα αποθηκεύουμε τα νέα κέντρα
        cv::Mat newCentroids= cv::Mat::zeros(k_,dims,CV_32F);
        //Δομή στην οποία αποθηκεύουμε για το κάθε κέντρο πόσοι SIFT Descriptors ανήκουν στο cluster του
        std::vector<int> clusterCounts(k_,0);


        //Για κάθε SIFT descriptor θα εξετάσουμε την απόσταση του απο κάθε κέντρο με σκοπό να το βάλουμε να ανήκει στο πλησιέστερο κέντρο
        for(int i=0;i<n;i++){
            const float* point= dataFloat.ptr<float>(i);
            float minDist= std::numeric_limits<float>::max(); // Παίρνουμε το μαξ που χωράει σε εναν float ωστε να κρατήσουμε στο τέλος και μετα τις συγκρισεις την πιο μικρή τιμη (απόσταστη από centroid) στη μεταβλήτη minDist
            int bestCluster=0; // Εδώ θα αποθηκεύουμε το κοντινότερο κέντρο κάθε φορά
            
            //Μέτρηση απόστασης από ολα τα κέντρα πλήθους k
            for(int c=0;c<k_;c++){
                float dist= squaredDistance(point, centroids_.ptr<float>(c), dims);
                if(dist<minDist){
                    minDist=min;
                    bestCluster=c;
                }

            }

            //Κρατάμε το πιο κοντινό κέντρο για το καθε SIFT Descriptor
            if(assignments[i]!=bestCluster){
                assignments[i] = bestCluster;
                changedAssignments++;
            }
            
        }
    }
}

}