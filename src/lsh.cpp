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
    unsigned int hash_val=0; // Αριθμός bucket στον οποιο θα αποθηκευτει η εικόνα
    
    //Το Hash Code για να υπολογιστει θα πρεπει να υπολογισουμε την προβολή του δθανυσματος x πανω σε ολες τις τυχαιες ευθειες LSHFunction
    for(int i=0;i<k;i++){
        float dot_product=0.0f;// Εσωτερικο γινομενο -- προβολη x στην τυχαια ευθεια toy LSHFunction
        for(size_t j=0;j<x.size();j++){
            dot_product +=x[j] * functions[i].v[j];
        }

        // h_i(x) = floor((v * x + t)/w)
        int h_i = static_cast<int>(std::floor((dot_product +functions[i].t) /w));
    
        // Συνδυάζουμε τα αποτελέσματα των k συναρτήσεων σε ένα ενιαίο unsigned int (απλό hashing)
        // Χρησιμοποιούμε μια τυχαία (αλλά σταθερή) μίξη, π.χ., τον αλγόριθμο FNV-1a ή μια απλή πρόσθεση/μετατόπιση
        hash_val ^= (h_i + 0x9e3779b9 + (hash_val << 6) + (hash_val >> 2));   
    }

    return hash_val;
}


LSHSearch::LSHSearch(const Database& db, int k, int L, float w, int seed): db_(db), k_(k), L_(L), w_(w) {

    //Παιρνουμε τα δεδομένα της βάσης
    const auto& items= db_.getItems();
    if(items.empty()){
        std::cout<< "Η βαση με τις εικονες ειναι αδεια. Τερματισμος LSH\n";
        return;
    }
    
    int dim= items[0].vec.size();// Διασταση BOVW διανυσματος

    //Προετοιμάζουμε την παραγωγή τυχαίων αριθμών
    std::mt19937 gen(seed);
    std::normal_distribution<float> d_normal(0.0f, 1.0f); // Κανονική κατανομή N(0,1) για το v
    std::uniform_real_distribution<float> d_uniform(0.0f, w_); // Ομοιόμορφη κατανομή [0, w) για το t

    //Δημιουργία των 
    families_.resize(L_);
    hash_tables_.resize(L_);

    std::cout << "[LSH] Δημιουργία " << L_ << " Hash Tables με k=" << k_ << " και w=" << w_ << "...\n";

    //Αρχικοποιώ τα struct LSHFamily πλήθους L_
    for(int l=0;l<L_;l++){
        families_[l].k=k_;
        families_[l].w=w_;
        families_[l].functions.resize(k_);

        //Για κάθε L θα φτιάξουμε k συανρτήσεις προβολής (τυχαιες ευθειες)
        for (int i= 0;i <k_;i++) {
            families_[l].functions[i].v.resize(dim);
            for (int j =0; j <dim;j++) {
                families_[l].functions[i].v[j] = d_normal(gen);
            }
            families_[l].functions[i].t= d_uniform(gen);
        }
    }


}

}