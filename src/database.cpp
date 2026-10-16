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
}
