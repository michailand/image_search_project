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

    std::string DatasetSplit::setTypeToString(SetType type){
        switch(type){
            case SetType::TRAIN: return "train";
            case SetType::VALIDATION: return "validation";
            case SetType::TEST: return "test";
            default: return "unknown";
        }
    }

    size_t DatasetSplit::totalSequences() const{
        return sequences_.size();
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


    std::vector<std::string> DatasetSplit::getSequences(SetType type) const{
        std::vector<std::string> result;

        for(const auto& seq: sequences_){
            if(seq.setType== type){
                result.push_back(seq.name);
            }
        }
        return result;
    }

    SetType DatasetSplit::getSetType(const std::string& sequenceName ) const {
        auto it=sequenceMap_.find(sequenceName);
        if(it!=sequenceMap_.end()){
            return it->second;
        }
        return SetType::UNKNOWN;
    }


}