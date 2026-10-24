#pragma once

#include <vector>
#include <string>
#include <map>
#include <random>
#include "database.hpp"
#include "exact_search.hpp"

namespace image_search {

struct LSHFunction{
    std::vector<float> v; //τυχαίο διάνυσμα προβολής
    float t; //τυχαία μετατόπιση
};

//Δομη η οποια κραταει μια ομαδα απο συναρτησεις hashing
struct LSHFamily{
    std::vector<LSHFunction> functions;
    int k; // Μας δειχνει ποσες LSHFunction περιεχει η δομη
    float w;

    

};
class LSHSearch {
public:



private:

    //βοηθητικη συναρτηση για ευρεση ευκλειδιας αποστασης
    static float squaredDistance(const std::vector<float>& a, const std::vector<float>& b);
};

}