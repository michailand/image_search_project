#include <iostream>
#include <opencv2/opencv.hpp>
#include "split.hpp"
#include "sift_extractor.hpp"
#include "kmeans.hpp"
#include "BoVW.hpp"
#include <cmath>

#include "database.hpp"
#include <algorithm>

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
            image_search::KMeans kmeans(10,40,1);
            cv::Mat centroids= kmeans.fit(sampledDesc);
            std::cout << "Centroids matrix shape: " << centroids.rows<< " x " << centroids.cols << "\n";
        }

        std::vector<std::string> testSequences= train;

        // --- Δοκιμή BoVW ---
        image_search::BoVW bovw;

        bool built= bovw.buildVocabulary("data/hpatches-sequences-release",testSequences,100,500,1,30);

        if(built){
            std::cout << "Vocabulary built successfully with " << bovw.computeVector(cv::Mat()).size() << " dimensions.\n";
        }
        else{
            std::cerr << "Failed to build vocabulary.\n";
        }

        const cv::Mat& vocabulary =bovw.getVocabulary();

        std::cout << "To vocabulary ftiaxthke kai exei rows= " << vocabulary.rows << " kai cols= " << vocabulary.cols << "\n";

        //Apothikeysi se arxeio toy vocabulary
        std::string vocabFile= "tests/vocabulary.bin";
        bool storeVocab= bovw.saveVocabulary(vocabFile);
        if(storeVocab){
            std::cout << "To vocabulary apothikeythke sto arxeio: " << vocabFile << "\n";
        }
        else{
            std::cerr << "Failed to save vocabulary to file: " << vocabFile << "\n";
        }

        //Τεστ για να δω αν το loadVocabulary δουλευει σωστα. 
        image_search::BoVW bovw2;
        bool loadVocab2= bovw2.loadVocabulary(vocabFile);
        if(!loadVocab2){
            std::cerr<<"ΠΡΟΣΟΧΗ!!! Το loadVocabulary απέτυχε για το αρχείο: "<<vocabFile<<"\n";
        }
        bool storeVocab2= bovw2.saveVocabulary("tests/vocabulary2.bin");
        if(!storeVocab2){
            std::cerr<<"ΠΡΟΣΟΧΗ!!! Το saveVocabulary απέτυχε για το αρχείο: tests/vocabulary2.bin\n";
        }   

        //Ελεγχος των δυο binary αρχειων
        int res= std::system("cmp tests/vocabulary.bin tests/vocabulary2.bin");
        if(res==0){
            std::cout << "Τα δυο αρχεία vocabulary.bin και vocabulary2.bin είναι ίδια.\n";
        }
        else{
            std::cerr << "ΠΡΟΣΟΧΗ!!! Τα δυο αρχεία vocabulary.bin και vocabulary2.bin ΔΕΝ είναι ίδια.\n";
        }

        std::string sampleImgPath= "data/hpatches-sequences-release/" + train[0] + "/1.ppm";

        std::cout << "Dokimi gia thn eikona: " << sampleImgPath << "\n";

        std::cout<< "exagogi SIFT descriptors gia to sampleImgPath \n";
        cv::Mat sampleDescriptors= image_search::extractSIFTDescriptors(sampleImgPath);

        std::cout << "To sampleDescriptors exei rows= " << sampleDescriptors.rows << " kai cols= " << sampleDescriptors.cols << "\n";


        std::vector<float> imageVector= bovw.computeVector(sampleDescriptors);
        std::cout << "To imageVector exei size= " << imageVector.size() << "\n";

        float sumDims=0.0f;
        std::cout << "To imageVector= [";
        for(std::size_t i=0;i<imageVector.size();i++){
            std::cout << imageVector[i] << " , ";
            sumDims+= imageVector[i];
        }
        std::cout << "]\n";

        std::cout << "To athroisma twn diastasewn tou imageVector= " << sumDims << "\n";

        //Δοκιμη για την δημιουργια της βασης δεδομενων
        std::cout << "Dokimi gia thn dimiourgia ths bashs dedomenwn \n";
        image_search::Database db;

        bool dbCreated= db.loadOrBuild("data/hpatches-sequences-release","data/mirflickr",split,image_search::SetType::VALIDATION,1000,bovw,"tests/database.bin");
    
        if(dbCreated){
            std::cout << "Η νεα βαση δεδομενων δημιουργηθηκε επιτυχως και αποθηκευτηκε στο αρχειο: tests/database.bin\n";
            
            const auto& items= db.getItems();
            std::cout << "Η νεα βαση δεδομενων περιεχει συνολικα " << items.size() << " αντικείμενα (εικόνες)\n";   
            for (size_t i = 0; i < std::min(size_t(3), items.size()); ++i) {
            std::cout << " Item " << i << " -> ID: " << items[i].id  << " Vector size: " << items[i].vec.size() << "\n";
        }
        }
        else{
            std::cerr << "ΠΡΟΣΟΧΗ!!! Η νεα βαση δεδομενων ΔΕΝ δημιουργηθηκε επιτυχως και ΔΕΝ αποθηκευτηκε στο αρχειο: tests/database.bin\n";
        }

    }
    else{
        std::cerr << "Failed to load split.txt file \n";
    }

    
    return 0;
}