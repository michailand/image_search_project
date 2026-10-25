#ifndef HYPERCUBE_HPP
#define HYPERCUBE_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <random>
#include <opencv2/opencv.hpp>
#include "database.hpp"


namespace image_search {

class HypercubeSearch{
public:
    //Constructor της HypercubeSearch
    // db: η βαση με τα διανυσματα bovw των εικονων
    // w: το πληθος των κομματιων που θα κοψουμε το εσωτερικο γινομενο του διανυσματος της εικονας με το τυχαιο διανυσμα x
    // M: Μέγιστος αριθμός υποψήφιων διανυσμάτων που συλλέγεις κατά την αναζήτηση
    // probes: Μέγιστος αριθμός κορυφών που επιχειρείς να εξετάσεις, συμπεριλαμβανομένης της κορυφής του query
    // seed: χρησιμοποιούμε ίδιο seed ωστε η αρχικοποιηση των τυχαιων παραμετρων να ειναι ιδια
    HypercubeSearch(const Database& db, int kproj, double w, int M, int probes, int seed);

private:
    const Database& db_;
    int kproj_;
    double w_;
    int M_;
    int probes_;
    int seed_;

};

}

#endif