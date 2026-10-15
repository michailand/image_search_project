#include "BoVW.hpp"
#include "sift_extractor.hpp"
#include "kmeans.hpp"
#include <iostream>
#include <fstream>
#include <limits>


namespace image_search{

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



}