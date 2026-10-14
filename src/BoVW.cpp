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
            
        }
    }
}



}