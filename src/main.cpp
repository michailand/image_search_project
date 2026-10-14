#include <iostream>
#include <opencv2/opencv.hpp>
#include "split.hpp"
#include "sift_extractor.hpp"
#include "kmeans.hpp"

int main(){
    std::cout << CV_VERSION << std::endl;

    image_search::DatasetSplit split;
    //Φορτωνουμε το split.txt και αποθηκευουμε τα sequences σε vector αναλογα τον τύπο (SetType) της ακολουθίας
    if(split.loadFromFile("split.txt")){
        auto train=split.getSequences(image_search::SetType::TRAIN);
        auto val=split.getSequences(image_search::SetType::VALIDATION);
        auto test=split.getSequences(image_search::SetType::TEST);

        std::cout << " --- Split results  ----" << std::endl;

        std::cout<< "Total sequences  " << split.totalSequences() << "\n";

        std::cout<< "Train sequences  " << train.size() << "\n";
        std::cout<< "Validation sequences  " << val.size() << "\n";
        std::cout<< "Test sequences  " << test.size() << "\n";

        // --- Δοκιμή SIFT στην 1η εικόνα της 1ης Train ακολουθίας ---
        if (!train.empty()) {
            std::string sampleSeq = train[0]; // π.χ. "i_ajuntament"
            std::string imgPath = "data/hpatches-sequences-release/" + sampleSeq + "/1.ppm";

            std::cout << "--- Testing SIFT on: " << imgPath << " ---\n";

            // 1. Εξαγωγή όλων των SIFT
            cv::Mat allDesc = image_search::extractSIFTDescriptors(imgPath);
            std::cout << "Total SIFT descriptors: " << allDesc.rows 
                      << " (Dimensions: " << allDesc.cols << ")\n";

            // 2. Τυχαία δειγματοληψία με όριο S = 500 και seed = 1
            cv::Mat sampledDesc = image_search::extractSIFTDescriptors(imgPath, 500, 1);
            std::cout << "Sampled descriptors (S=500, seed=1): " << sampledDesc.rows << "\n";

            //3.Dokimi kmeans
            image_search::KMeans kmeans(10,20,1);
            cv::Mat centroids= kmeans.fit(sampledDesc);
            std::cout << "Centroids matrix shape: " << centroids.rows<< " x " << centroids.cols << "\n";
        }


    }
    else{
        std::cerr << "Failed to load split.txt file \n";
    }

    
    return 0;
}