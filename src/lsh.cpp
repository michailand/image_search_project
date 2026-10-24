#include "lsh.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>

namespace image_search {
//βοηθητικη συνάρτηση για την ευρεση της ευκλειδιας αποστασης 
float LSHSearch::squaredDistance(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}

//Συναρτηση για τον υπολογισμο του Hash Code για ένα διάνυσμα x
unsigned int LSHFamily::computeHash(const std::vector<float>& x) const {

    //Το Hash Code για να υπολογιστει θα πρεπει να υπολογισουμε την προβολή του δθανυσματος x πανω σε ολες τις τυχαιες ευθειες LSHFunction
    for(int i=0;i<k;i++){
        float dot_product=0.0f;// Εσωτερικο γινομενο -- προβολη x στην τυχαια ευθεια toy LSHFunction
        for(size_t j=0;j<x.size();j++){
            dot_product +=x[j] * functions[i].v[j];
        }
    }
}

}