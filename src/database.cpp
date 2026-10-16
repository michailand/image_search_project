#include "database.hpp"
#include "sift_extractor.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>



Database::bool loadOrBuild(const std::string& hpatchesDir, const std::string& mirflickDir, const DatasetSplit& split, SetType setType, int numMirflickr, BoVW& bovw, const std::string& cacheFilename ){

    //Ελεγχος αν υπαρχει ηδη η βαση δεδομενων σε binary αρχειο
    if(fs::exists(cacheFilename)){
        std::cout<< "Φορτωση της προυπαρχουσας βασης δεδομενων απο το αρχειο: "<< cacheFilename<<"\n";
        bool loadedDatabase= loadBinary(cacheFilename);
        if(loadedDatabase){
            std::cout<< "Η προυπαρχουσα βαση δεδομενων φορτωθηκε επιτυχως απο το αρχειο: "<< cacheFilename<<"\n";
            return true;

        }
        else{
            std::cerr<< "ΠΡΟΣΟΧΗ!!! Η προυπαρχουσα βαση δεδομενων ΔΕΝ φορτωθηκε επιτυχως απο το αρχειο: "<< cacheFilename<<"\n";
            std::cerr<< "Θα δημιουργηθει νεα βαση δεδομενων και θα αποθηκευτει στο αρχειο: "<< cacheFilename<<"\n";

        }
    }

    auto sequences= split.getSequences(setType);

    //αδειαζω το vector items_ για να δημιουργησω νεα βαση δεδομενων
    items_.clear();

    std::cout<< "Εναρξη δημιουργιας νεας βασης δεδομενων για " << sequences.size() << " ακολουθιες HPatches και θορυβο " << numMirflickr << " εικόνες θορύβου mirFlickr\n";  
    int hpatchesCount=0;
    //ξεκιναμε με την προσθηκη των ακολουθιων HPatches (από Ι2 έως και Ι6)
    for(const auto& seqName: sequences){
        for(int imgIdx=2;imgIdx<=6;imgIdx++){
            //Για καθε ακολουθια φτιαχνουμε το path της εικονας
            std::string imgPath= hpatchesDir+ "/" + seqName + "/" + std::to_string(imgIdx) + ".ppm";
            //ΕΞΑΓΩΓΗ SIFT descriptors για την εικονα ωστε μετα να μπορεσουμε να φτιαξουμε το BoVW vector της
            cv::Mat desc= extractSIFTDescriptors(imgPath);

            if(!desc.empty()){
                //Παιρνουμε το BoVW vector για την εικονα
                std::vector<float> vec= bovw.computeVector(desc);
                std::string itemId= seqName + "_" + std::to_string(imgIdx);

                //Προσθηκη του νεου αντικειμενου στη βαση δεδομενων
                items_.push_back({itemId, vec});
                hpatchesCount++;
            }
        }
    }

    std::cout<<"Φορτωθηκαν "<< hpatchesCount << " εικόνες απο τη βαση δεδομενων HPatches\n";

    //Τωρα θα προσθεσουμε και τις εικόνες θορύβου mirFlickr
    int mirflickrCount=0;
    if(fs::exists(mirflickDir)){
        for(const auto& entry : fs::directory_index_iterator(mirflickDir)){
            //Ελεγχος αν εχουμε φτασει στο οριο των numMirflickr
            if(mirflickrCount>= numMirflickr){
                break;
            }

            if(entry.is_regular_file()){
                std::string imgPath= entry.path().string();
                cv::Mat desc= extractSIFTDescriptors(imgPath);

                //αρχικά ελέγχουμε αν η εικονα εχει descreiptors και αν ναι τοτε προχωραμε στην δημιουργια του BoVW vector της
                // και αυξανουμε και το μετρητή mirflickrCount για να ξερουμε ποσες εικόνες θορύβου εχουμε προσθεσει στη βαση δεδομενων
                if(!desc.empty()){
                    std::vector<float> vec= bovw.computeVector(desc);
                    std::string filename= entry.path().filename().string();
                    mirflickrCount++;
                }
            }
        }
    }

}
