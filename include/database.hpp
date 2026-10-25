#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>
#include "BoVW.hpp"
#include "split.hpp"
#include <cstddef>


namespace image_search{

struct DatabaseItem {
    std::string id; 
    std::vector<float> vec;
}; 

class Database {
public:
    
    //Συναρτηση για την φορτωση αν υπαρχει ηδη ή την δημιουργια της βασης δεδομενων. 
    bool loadOrBuild(const std::string& hpatchesDir, const std::string& mirflickDir, const DatasetSplit& split, SetType setType, int numMirflickr, BoVW& bovw, const std::string& cacheFilename );

    //Φορτωση της βάσης δεδομένων σε binary αρχείο
    bool loadBinary(const std::string& filename);
    
    //Αποθηκευση της βάσης δεδομένων σε binary αρχείο
    bool saveBinary(const std::string& filename) const;

    //getter για το vector items_ που περιέχει τα BoVW vectors και τα αντίστοιχα ID τους
    const std::vector<DatabaseItem>& getItems() const {
        return items_;
    }

    //Βοηθητικη συνάρτηση για να μας επιστρεφει το BoVW διανυσμα μιας εικονας της βασης
    const std::vector<float>& getVector(std::size_t index) const{
        return items_[index].vec;
    }

    //Επιστρεφει το size του database
    std::size_t size() const{
        return items_.size();
    }

private:
    //Vector για την αποθηκευση των διανυσματων των εικόνων (BoVW vectors) και των αντίστοιχων ID τους.
    std::vector<DatabaseItem> items_;
};

}