#pragma once

#include <vector>
#include <string>
#include <map>
#include <opencv2/opencv.hpp>
#include "database.hpp"
#include "exact_search.hpp"

namespace image_search {

//Δομή για να κρατάμε τα συμπιεσμένα διανύσματα των εικόνων PQ Codes
struct PQItem{
    std::string id;
    std::vector<unit8_t> pq_code;
};



class IVFPQ {
public:


private:
    const Database& db_; // Η βαση με τα BoVW (Visual Words)
    int kclusters_; // Τα coarse clusters του db_ των Visual Words
    int M_; // Το πληθος των τμηματων στο οποίο θα χωρίσω τα residuals(δηλαδη της απόστασης απο το coarse κεντρο της ) της καθε εικόνας 

    static float squaredDistance(const std::vector<float>& a, const std::vector<float>& b);

};

}