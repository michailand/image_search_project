#pragma once
#include <string>
#include <vector>
#include <unordered_map>


namespace image_search {

//Τυπος των δεδομενων
enum class SetType{
    TRAIN,
    VALIDATION,
    TEST,
    UNKNOWN
};


//Struct στο οποιο θα αποθηκευεται το καθε στοιχειο καθως και ο τυπος του
struct SequenceInfo{
    std::string name;
    SetType setType;
};

class DatasetSplit {
public:
    DatasetSplit()=default; //επιβάλω την χρηση του default constructor
    
    //διαβαζει το αρχειο split.txt και σε επιτυχια επιστρεφει 1 αλλιως 0
    bool loadFromFile(const std::string &filename);


    //βοηθητικές συναρτήσεις
    static SetType stringToSetType(const std::string& str);


private:
    //Δημιουργουμε ενα hash και ενα vector για να κραταμε τα δεδομενα του split
    std::vector<SequenceInfo> sequences_;
    std::unordered_map<std::string,SetType> sequenceMap_;
};

}