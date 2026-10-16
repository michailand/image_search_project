#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "BoVW.hpp"
#include "split.hpp"



namespace image_search{

class Database {
public:
    
    //Συναρτηση για την φορτωση αν υπαρχει ηδη ή την δημιουργια της βασης δεδομενων. 
    bool loadOrBuild(const std::string& hpatchesDir, const std::string& mirflickDir, const DatasetSplit& split, SetType setType, int numMirflickr, BoVW& bovw, const std::string& cacheFilename );

    //Φορτωση της βάσης δεδομένων σε binary αρχείο
    bool loadBinary(const std::string& filename); 
}

}