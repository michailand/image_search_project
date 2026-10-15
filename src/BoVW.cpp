#include "BoVW.hpp"
#include "sift_extractor.hpp"
#include "kmeans.hpp"
#include <iostream>
#include <fstream>
#include <limits>


namespace image_search{


//Η συνάρτηση buildVocabulary είναι υπεύθυνη για την δημιουργία του λεξικού μας. Αρχικά θα συλλέξει όλα τα SIFT Descriptors από τις εικόνες εκπαίδευσης και στη συνέχεια θα καλέσει την συνάρτηση kmeans για να δημιουργήσει το λεξικό μας.
bool BoVW::buildVocabulary(const std::string& hpatchesDir,const std::vector<std::string>& trainSequences,int KClusters,int maxDescriptorPerImage,unsigned int seed,int maxIters ){

    //Pinakas ston opoio apothikeyo ta SIFT Descriptors olon ton eikonon
    cv::Mat allDescriptors;

    //Σύνολο των εικόνων (Εχει 6 το καθε πακετο)
    int totalImages= trainSequences.size()*6;
    int processed=0;
    std::cout<< "Αρχίζει η συλλογή των SIFT Descriptors απο συνολικά "<< totalImages << " εικονες εκπαίδευσης\n";

    for(const auto& seqName : trainSequences){
        for(int imgIdx=1;imgIdx<=6;imgIdx++){
            std::string imgPath= hpatchesDir + "/" +seqName + "/" + std::to_string(imgIdx) +".ppm";
       
            //Κλήση της συνάρτησης extractSIFTDescriptors για εξαγωγη έως και S decriptors με seed το οποιο το αλλαζουμε καθε φορα +1
            cv::Mat desc= extractSIFTDescriptors(imgPath,maxDescriptorPerImage,seed + processed);
            //Αν δεν ειναι άδειο το αρχείο με τους descriptors τοτε το προωθω στο Mat allDescriptors
            if(!desc.empty()){
                allDescriptors.push_back(desc);
            }

            processed++;

            if(processed== totalImages){
                std::cout << "Τελος Συλλογης των SIFT Descriptors. Επεξεργάστηκαν συνολικά  "<< processed << " εικονες από τις συνολικά " << totalImages << " εικονες εκπαίδευσης\n";
            }
            
        }
    }

    if(allDescriptors.empty() || allDescriptors.rows < KClusters){
        std::cerr << "Δεν βρέθηκαν SIFT Descriptors για τις εικόνες εκπαίδευσης. Ελέγξτε τα paths των εικόνων.\n";
        return false;
    }

    //Τωρα θα καλέσω την συνάρτηση kmeans για να δημιουργήσω το λεξικό μου
    std::cout << "Αρχίζει η διαδικασία KMeans για τη δημιουργία του λεξικού με " << KClusters << " clusters\n";
    KMeans kmeans(KClusters,maxIters,seed);
    vocabulary_ = kmeans.fit(allDescriptors);

    return !vocabulary_.empty();
}



//Παιρνει ως ορίσματα δυο διανυσματα (a & b) (vector) και το μέγεθος τους (dims) δηλαδ�� τις διαστάσεις του και επιστρέφει την απόσταση μεταξύ τους 
float BoVW::squaredDistance(const float* a,const float* b, int dims){
    float res=0.0f;
    for(int i=0;i<dims;i++){
        float tmp= a[i]-b[i];
        tmp= tmp *tmp;
        res+=tmp;
    }
    return res;
}


//Με αυτή την συνάσρτηση δίνουμε μία-μια εικόνα δηλαδη τα SIFT Descriptors της και μας επιστρέφει ένα διάνυσμα διαστάσεων KClusters το οποίο αναπαριστά την εικόνα με βάση το λεξικό μας. Το διάνυσμα αυτό θα χρησιμοποιηθεί για την αναζήτηση εικόνων με βάση την ομοιότητα των διανυσμάτων τους.
std::vector<float> BoVW::computeVector(const cv::Mat& descriptors) const{
    //πλήθος των clusters που έχουμε στο λεξικό μας 
    int K= vocabulary_.rows;
    //Πινακας ο οποιος θα εχει το τελικό διανυσμα της εικονας με την μορφη του λεξικου vocabulary_ που φτιαξαμε πλήθους Κ λέξεων. Αρα αν η αρχικη μας λέξη ειχε 500 sift descriptors και το λεξικό μας ειχε 100 clusters τότε το τελικό διανυσμα θα εχει 100 διαστάσεις. 
    std::vector<float> hist(K,0.0f);

    //Αν το descriptors η το vocabulary_ ειναι άδειο τότε επιστρεφουμε αδειο vector hist
    if(descriptors.empty() || vocabulary_.empty()){
        return hist;
    }

    //Για σιγουρια ελέγχουμε πάλι αν οι desxriptors ειναι τυπου CV_32F
    cv::Mat descFloat;
    if(descriptors.type() != CV_32F){
        descriptors.convertTo(descFloat,CV_32F);
    }
    else{
        descFloat= descriptors;
    }

    int descriptorRows= descFloat.rows; // Πλήθος των SIFT Descriptors που εχει η εικόνα μας
    int descriptorCols= descFloat.cols; // Dimension για τους SIFT Descriptors--- εχουμε ορισει 128 διαστασεις

    //Για ολους τους SIFT Descriptors 
    for(int i=0;i<descriptorRows;i++){
        //Παιρνω το row του καθε SIFT Descriptor το οποιο περιεχει 128 διαστασεις
        const float* descriptorRow= descFloat.ptr<float>(i);
        float minDist= std::numeric_limits<float>::max(); //παιρνω το μαξ πο χωραει ενας φλοατ 
        int bestCluster=0; // Εδω θα βαζουμε το κοντινοτερο centroid (visual word) για τον καθε SIFT Descriptor

        //Για ολα τα centroids του λεξικου 
        for(int c=0;c<K;c++){
            //Απόσταση του καθε SIFT Descriptor απο το καθε centroid του λεξικου και ευρεση αυτου με την μικροτερη αποσταση. Αφου το βρω τοτε ο sift descriptor ανηκει στο cluster του συγκεκριμενου centroid και αυξάνω το πλήθος του cluster αυτού κατα 1
            float dist=squaredDistance(descriptorRow,vocabulary_.ptr<float>(c),descriptorCols);
            if(dist<minDist){
                minDist= dist;
                bestCluster= c;
            }
        }

        hist[bestCluster]+=1.0f; 
    }

    //Τωρα θα κανω κανονικοποίηση ώστε το άθροισμα τους να βγαινει 1
    for(int i=0;i<K;i++){
        float res= hist[i]/descriptorRows;
        hist[i]=res;
    }

    return hist;



}


bool BoVW::saveVocabulary(const std::string& filename) const {
    if(vocabulary_.empty()) {
        std::cerr << "Προσοχή!!! Το Vocabulary ειναι αδειο.\n";
        return false;
    }

    std::ofstream outFile(filename, std::ios::binary);
    if(!outFile) {
        std::cerr << "Προσοχή!!! Δεν μπορει να ανοιξει το αρχείο για αποθήκευση του Vocabulary: " << filename << "\n";
        return false;
    }

    //Πρωτα θα αποθηκευσω τις διαστασεις του vocabulary_ (rows και cols) και μετα τα δεδομενα του vocabulary_
    int rows =vocabulary_.rows;
    int cols =vocabulary_.cols;
    outFile.write(reinterpret_cast<const char*>(&rows), sizeof(int));
    outFile.write(reinterpret_cast<const char*>(&cols), sizeof(int));
    outFile.write(reinterpret_cast<const char*>(vocabulary_.data), rows * cols * sizeof(float));

    return true;
}



}