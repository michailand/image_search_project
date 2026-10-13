#include "split.hpp"
#include <fstream>
#include <sstream>
#include <iostream>




namespace image_search {

    SetType DatasetSplit::stringToSetType(const std::string& str){
        if(str== "train") return SetType::TRAIN;
        if(str== "validation") return SetType::VALIDATION;
        if(str== "test") return SetType::TEST;

        return SetType::UNKNOWN;
    }

    bool DatasetSplit::loadFromFile(const std::string &filename){
        //ανοίγουμε το αρχείο split και ελεγχουμε αν το ανοιγμα εγινε σωστα 
        std::ifstream file(filename);
        if(!file.is_open()){
            std::cerr<< "ERROR:Couldn't open Split file  " <<filename << std::endl;
            return false; 
        }

        //ξεκινάμε να διαβάζουμε το αρχείο γραμμη-γραμμη
        std::string line;
        while(std::getline(file,line)){
            if(line.empty()) continue;
            //θα διαβασω την γραμμη ωστε να την σπασω σε δυο μερη
            std::istringstream iss(line);
            std::string seqName,setStr;
            if(iss>> seqName>> setStr){
                SetType type= stringToSetType(setStr);
                sequences_.push_back({seqName,type});
                sequenceMap_[seqName]=type;
            }
        }

        file.close();
        
        return true;
    }
}