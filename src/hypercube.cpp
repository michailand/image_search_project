#include "hypercube.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>
#include <queue>

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

    }

}