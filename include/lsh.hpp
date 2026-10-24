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

    //Η συνάρτηση αυτή παίρνει το διάνυσμα μιας εικόνας, υπολογίζει το αποτέλεσμα κάθε συνάρτησης, Συνδυάζει τα αποτελέσματα και επιστρέφει έναν ενιαίο unsigned int.
    unsigned int computeHash(const std::vector<float>& x) const;

};
class LSHSearch {
public:



private:

    //βοηθητικη συναρτηση για ευρεση ευκλειδιας αποστασης
    static float squaredDistance(const std::vector<float>& a, const std::vector<float>& b);
};

}