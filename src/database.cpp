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

    //ξεκιναμε με την προσθηκη των ακολουθιων HPatches (από Ι2 έως και Ι6)
    for(const auto& seqName: sequences){
        for(int imgIdx=2;imgIdx<=6;imgIdx++){
            std::string imgPath= hpatchesDir+ "/" + seqName + "/" + std::to_string(imgIdx) + ".ppm";
            
        }
    }


}
