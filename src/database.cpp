#include "database.hpp"
#include "sift_extractor.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace image_search{

bool Database::loadOrBuild(const std::string& hpatchesDir, const std::string& mirflickDir, const DatasetSplit& split, SetType setType, int numMirflickr, BoVW& bovw, const std::string& cacheFilename ){

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
        for(const auto& entry : fs::directory_iterator(mirflickDir)){
            //Ελεγχος αν εχουμε φτασει στο οριο των numMirflickr
            if(mirflickrCount>= numMirflickr){
                break;
            }

            if(entry.is_regular_file()){
                std::string ext= entry.path().extension().string();
                if(ext == ".jpg" || ext ==".JPG" || ext==".jpeg" || ext==".JPEG"){
                    std::string imgPath= entry.path().string();
                    cv::Mat desc= extractSIFTDescriptors(imgPath);
                }
                //αρχικά ελέγχουμε αν η εικονα εχει descreiptors και αν ναι τοτε προχωραμε στην δημιουργια του BoVW vector της
                // και αυξανουμε και το μετρητή mirflickrCount για να ξερουμε ποσες εικόνες θορύβου εχουμε προσθεσει στη βαση δεδομενων
                if(!desc.empty()){
                    std::vector<float> vec= bovw.computeVector(desc);
                    std::string filename= entry.path().filename().string();
                    mirflickrCount++;

                    items_.push_back({filename, vec});
                    
                    if(mirflickrCount % 1000==0 ){
                        std::cout<< "Προστέθηκαν "<< mirflickrCount << " εικόνες θορύβου mirFlickr στη βαση δεδομενων\n";
                        std::cout<< "Απομένουν "<< (numMirflickr - mirflickrCount) << " εικόνες θορύβου mirFlickr για προσθήκη στη βαση δεδομενων\n";
                    }
                }
            }
        }
    }
    else{
        std::cerr<< "ΠΡΟΣΟΧΗ!!! Ο φάκελος mirFlickr δεν βρέθηκε: "<< mirflickDir << "\n";
    }

    std::cout<<"Φορτωθηκαν συνολικα "<< items_.size() << " εικόνες στη βαση δεδομενων (HPatches + mirFlickr)\n";


    //edo tha mpei kodikas gia apothikeysi ths bashs dedomenwn se binary arxeio
    bool saved= saveBinary(cacheFilename);
    if(saved){
        std::cout<< "Η νεα βαση δεδομενων αποθηκευτηκε επιτυχως στο αρχειο: "<< cacheFilename<<"\n";
    }
    else{
        std::cerr<< "ΠΡΟΣΟΧΗ!!! Η νεα βαση δεδομενων ΔΕΝ αποθηκευτηκε επιτυχως στο αρχειο: "<< cacheFilename<<"\n";
    } 

    return !items_.empty();
}


bool Database::saveBinary(const std::string& filename) const{
    std::ofstream out(filename, std::ios::binary);
    if(!out.is_open()){
        std::cerr<< "ΠΡΟΣΟΧΗ!!! Αδυναμία ανοίγματος αρχείου για αποθήκευση: "<< filename << "\n";
        return false;
    }

    //plitos antikeimenon gia grapsimo
    size_t totalItems= items_.size();
    //Πρωτη θεση του αρχειου κραταει τα  metadata δηλαδη το μεγεθος του αρχειου
    out.write(reinterpret_cast<const char*>(&totalItems), sizeof(size_t));

    for (const auto& item : items_) {
        // Γράφουμε το ID (μέγεθος string + χαρακτήρες)
        size_t idSize = item.id.size();
        out.write(reinterpret_cast<const char*>(&idSize), sizeof(size_t));
        out.write(item.id.data(), idSize);

        // Γράφουμε το διάνυσμα (μέγεθος vector + δεδομένα floats)
        size_t vecSize = item.vec.size();
        out.write(reinterpret_cast<const char*>(&vecSize), sizeof(size_t));
        out.write(reinterpret_cast<const char*>(item.vec.data()), vecSize * sizeof(float));
    }

    return true;
    
}
    


bool Database::loadBinary(const std::string& filename){
    std::ifstream in(filename, std::ios::binary);
    if(!in.is_open()){
        std::cerr<< "ΠΡΟΣΟΧΗ!!! Αδυναμία ανοίγματος αρχείου: "<< filename << "\n";
        return false;
    }

    size_t totalItems = 0;
    in.read(reinterpret_cast<char*>(&totalItems), sizeof(size_t));
    if (totalItems == 0 || totalItems > 500000) return false;

    items_.clear();
    items_.resize(totalItems);

    for (size_t i = 0; i < totalItems; ++i) {
        size_t idSize = 0;
        in.read(reinterpret_cast<char*>(&idSize), sizeof(size_t));
        items_[i].id.resize(idSize);
        in.read(items_[i].id.data(), idSize);

        size_t vecSize = 0;
        in.read(reinterpret_cast<char*>(&vecSize), sizeof(size_t));
        items_[i].vec.resize(vecSize);
        in.read(reinterpret_cast<char*>(items_[i].vec.data()), vecSize * sizeof(float));
    }

    return true;



}


}