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

    //Πρωτα βρίσκουμε το εσωτερικο γινομενο δηλαδη την προβολή του διάνυσματος BoVW στην ευθεια xs
    for(int i=0;i<kproj_; i++){
       float dot_product = 0.0f;
        for (size_t j =0;j <x.size(); j++) {
            dot_product+= x[j] * functions_[i].v[j];
        } 
    

        // h_i(x) = floor((v * x + t) / w)
        int h_i = static_cast<int>(std::floor((dot_product + functions_[i].t) / w_));

        uint32_t mixed = static_cast<uint32_t>(h_i) ^ functions_[i].salt;
        mixed ^= (mixed >> 16);
        mixed *= 0x85ebca6b;
        mixed ^= (mixed >> 13);
        uint32_t bit = mixed & 1u;

        vertex = (vertex << 1) | bit;
    }

    return vertex;
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

    for(const auto& item : items){
        uint32_t vertex= computeVertex(item.vec);
        hypercube_table_[vertex].push_back(item);
    }

    std::cout << "[Hypercube] Εισήχθησαν " << db_.size() << " διανύσματα σε " << hypercube_table_.size() << " ενεργές κορυφές.\n";

}

//Με αυτη τη συνάρτηση θα βρουμε τις γειτονικές κορυφές του Hypercube που απέχουν συγκεκριμένο αριθμό bits από την αρχική κορυφή, δηλαδή έχουν συγκεκριμένη απόσταση Hamming
//dist: Πόσα bits θέλουμε να αντιστρέψουμε. Για παραδειγμα αν dist = 2 ψάχνουμε κορυφές που διαφέρουν σε ακριβώς 2 bits.
//startBit: Από ποια θέση bit ξεκινάμε να εξετάζουμε ώστε να αποφεύγουμε διπλούς συνδυασμούς
//maxProbes: Το μέγιστο πλήθος κορυφών που επιτρέπεται να αποθηκευτούν.
//outVertices: Το vector στο οποίο αποθηκεύονται οι κορυφές που δημιουργούνται.
//kproj: Το πλήθος των bits της κορυφής. Πχ αν είναι 4, έχουμε 2^4=16 κορυφές
void HypercubeSearch::generateHammingNeighbors(uint32_t vertex, int kproj, int dist, int startBit,size_t maxProbes, std::vector<uint32_t>& outVertices) {
    //Συνθηκη τερματισμού αναδρομής
    if(outVertices.size() >= maxProbes){
        return;
    }
    
    for(int bit=startBit; bit<=kproj-dist; bit++){
        //Αντιστρεφουμε καθε φορα ενα bit ολο και πιο αριστερα 
        uint32_t flipped= vertex^ (1u<<bit);
        generateHammingNeighbors(flipped, kproj, dist - 1, bit + 1, maxProbes, outVertices);
        if (outVertices.size() >= maxProbes) return;
    }


}
std::vector<SearchResult> HypercubeSearch::search(const std::vector<float>& queryVec, int topK) const {
    std::vector<SearchResult> results;

    //Βρισκουμε σε ποια κορυφη του υπερκυβου θα ανηκε το query 
    uint32_t queryVertex= computeVertex(queryVec);

    //ΑΠοθηκευουμε σε vector το μεγιστο αριθμο ( probes_ ) των κορυφων που προκειται να εξετασουμε
    std::vector<uint32_t> verticesToProbe;
    verticesToProbe.reserve(probes_);

    for (int dist = 0; dist <= kproj_ && verticesToProbe.size() < static_cast<size_t>(probes_);dist++){
        generateHammingNeighbors(queryVertex, kproj_, dist, 0, static_cast<size_t>(probes_), verticesToProbe);
    }

}

}