#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <random>
#include <cstdint>
#include "database.hpp"
#include "exact_search.hpp"

namespace image_search {

//Πρεπει να φτιάξουμε μια δομή για να κραταμε τα τυχαια διανύσματα πανω στα οποια θα προβαλλεται η BoVW εικονα
struct HCFunction{
    std::vector<float> v;//Τυχαιο διανυσμα με τιμες στο διαστημα (0,1)
    float t; // Τυχαια μετατοπιση 
    uint32_t  salt; //Τυχαιο salt για καλυτερο mapping
} ;

class HypercubeSearch{
public:
    //Constructor της HypercubeSearch
    // db: η βαση με τα διανυσματα bovw των εικονων
    // w: το πληθος των κομματιων που θα κοψουμε το εσωτερικο γινομενο του διανυσματος της εικονας με το τυχαιο διανυσμα x
    // M: Μέγιστος αριθμός υποψήφιων διανυσμάτων που συλλέγεις κατά την αναζήτηση
    // probes: Μέγιστος αριθμός κορυφών που επιχειρείς να εξετάσεις, συμπεριλαμβανομένης της κορυφής του query
    // seed: χρησιμοποιούμε ίδιο seed ωστε η αρχικοποιηση των τυχαιων παραμετρων να ειναι ιδια
    HypercubeSearch(const Database& db, int kproj, float w, int M, int probes, int seed);

    //Συναρτηση αναζητησης query εικονας
    std::vector<SearchResult> search(const std::vector<float>& queryVec, int topK = 10) const;

private:
    const Database& db_;
    int kproj_; //αριθμός των bits που χρησιμοποιεί ο Hypercube για να αντιστοιχίσει κάθε διάνυσμα σε μία κορυφή του υπερκύβου. πχ αν kproj-=3 τοτε θα εχω συνολικα 8 κορυφες στον υπερκυβο
    double w_;
    int M_;
    int probes_;

    //Πινακας στον οποιο αποθηκευουμε τα τυχαια διανυσματα
    std::vector<HCFunction> functions_;
    
    // Κάθε κορυφή του υπερκύβου (bitstring αποθηκευμένο σε uint32_t) 
    // αντιστοιχεί σε μια λίστα από DatabaseItem
    std::unordered_map<uint32_t, std::vector<DatabaseItem>> hypercube_table_;


    // Υπολογίζει την κορυφή (d' bits) στην οποία προβάλλεται το διάνυσμα x
    uint32_t computeVertex(const std::vector<float>& x) const;
  

    // Παράγει αναδρομικά τις γειτονικές κορυφές σε απόσταση Hamming = dist
    static void generateHammingNeighbors(uint32_t vertex, int kproj, int dist, int startBit,size_t maxProbes, std::vector<uint32_t>& outVertices);

    //Βοηθητικη συναρτηση για τον υπολογισμο της ευκλειδιας αποστασης δυο διανυσματων
    static float squaredDistance(const std::vector<float>& a, const std::vector<float>& b);
};

}
