#pragma once
#include <string>



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
};

}