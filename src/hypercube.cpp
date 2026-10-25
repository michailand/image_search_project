#include "hypercube.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <queue>

namespace image_search {

float HypercubeSearch::squaredDistance(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0.0f;
    for (size_t i=0;i<a.size();i++) {
        float diff =a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}

//Συναρτηση για να υπολογιζουμε σε ποια κορυφή του υπερκύβου θα τοποθετηθεί ένα διάνυσμα BoVW.
uint32_t HypercubeSearch::computeVertex(const std::vector<float>& x) const {
    //Κορυφη του υπερκυβου στην οποια θα μπει το διάνυσμα BoVW
    uint32_t vertex=0;

    for(int i=0;i<kproj_; i++){
       float dot_product = 0.0f;
        for (size_t j = 0; j < x.size(); ++j) {
            dot_product += x[j] * functions_[i].v[j];
        } 
    }


}



HypercubeSearch::HypercubeSearch(const Database& db, int kproj, float w, int M, int probes, int seed): db_(db),kproj_(kproj),w_(w),M_(M),probes_(probes) {

    const auto& items= db_.getItems();
    if(items.empty()){
        std::cout<< "ΠΡΟΣΟΧΗ!!! Το πλήθος εικόνων της βασης ειναι ΜΗΔΕΝΙΚΟ\n";
        return;
    }

    size_t dim= items[0].vec.size(); // Διαστασεις BoVW διανυσματων. Θα μου χρειαστει να αρχικοποιησω τις τυχαιες ευθειες προβολης


    //Αρχικοποιηση της γεννητριας τυχαιων αριθμων. Σκοπος μας ειναι καθε φορα που αρχικοποιουμε να εχουμε ίδια αποτελεσματα  
    std::mt19937 gen(seed);
    std::normal_distribution<float> d_normal(0.0f, 1.0f);
    std::uniform_real_distribution<float> d_uniform(0.0f, w_);
    std::uniform_int_distribution<uint32_t> d_salt(1u, 0xFFFFFFFFu);


    //Επειδη kproj ειναι ο αριθμός των bits που χρησιμοποιεί ο Hypercube για να αντιστοιχίσει κάθε διάνυσμα σε μία κορυφή του υπερκύβου τοτε θα χρειαζομαστε kproj πληθος τυχαιων διανυσμάτων
    //καθε εικονα προβαλλεται σε ενα διανυσμα και το εσωτερικο γινομενο (δηλαδη η προβολη) μετατρεπεται σε 1 bit . αρα αν kproj=3 τοτε θελω 3 bits αρα 3 τυχαια διανυσματα για την προβολη
    //Αρχικοποιησει των kproj διανυσματων για την  προβολή
    functions_.resize(kproj_);
    for(int i=0;i<kproj_;i++){
        functions_[i].v.resize(dim);
        for(size_t j=0;j<dim;j++){
            functions_[i].v[j] = d_normal(gen);
        }
        functions_[i].t = d_uniform(gen);
        functions_[i].salt = d_salt(gen);
    }

    //τωρα θα προβαλλουμε όλες τις εικόνες της βασης στον υπερκυβο
    std::cout << "[Hypercube] Δημιουργία Υπερκύβου με kproj=" << kproj_ << ", w=" << w_ << ", M=" << M_ << ", probes=" << probes_ << "\n";


}

}