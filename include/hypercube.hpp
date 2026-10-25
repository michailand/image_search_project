#ifndef HYPERCUBE_HPP
#define HYPERCUBE_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <random>
#include <opencv2/opencv.hpp>
#include "database.hpp"


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

private:
    const Database& db_;
    int kproj_;
    double w_;
    int M_;
    int probes_;
  
    //Βοηθητικη συναρτηση για τον υπολογισμο της ευκλειδιας αποστασης δυο διανυσματων
    static float squaredDistance(const std::vector<float>& a, const std::vector<float>& b);
};

}

#endif