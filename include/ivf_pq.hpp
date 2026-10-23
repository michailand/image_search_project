#pragma once

#include <vector>
#include <string>
#include <map>
#include <opencv2/opencv.hpp>
#include "database.hpp"
#include "exact_search.hpp"
#include "kmeans.hpp"

namespace image_search {

//Δομή για να κρατάμε τα συμπιεσμένα διανύσματα των εικόνων PQ Codes
struct PQItem{
    std::string id;
    std::vector<uint8_t> pq_code;
};



class IVFPQ {
public:
    IVFPQ(const Database& db, int kclusters, int M, int nbits, int seed); //Αρχικοποιησει της μεθοδου IVFPQ και δημιουργια των 

private:
    const Database& db_; // Η βαση με όλες τις εικόνες με τα BoVW βάσει των  Visual Words
    int kclusters_; // Τα coarse clusters του db_ των Visual Words
    int M_; // Το πληθος των τμηματων στο οποίο θα χωρίσω τα residuals(δηλαδη της απόστασης απο το coarse κεντρο της ) της καθε εικόνας 
    int nbits_; //Το πληθος των κέντρων του καθε Μ_ τμήματος (πχ nbits=8 => 2^8=256 κεντρα)
    int num_centroids_per_subspace_; // Το πληθος των κέντρων του καθε Μ_ τμήματος (πχ nbits=8 => 2^nbits=256 κεντρα)
    int sub_dim_; // Διαστάσεις κάθε υποδιανύσματος

    cv::Mat coarse_centroids_; // Ta coarse centroids του Database db_

    // Inverted Index: Κάθε συστάδα (0...kclusters-1) έχει τα δικά της PQItems
    std::map<int, std::vector<PQItem>> index_;

    static float squaredDistance(const std::vector<float>& a, const std::vector<float>& b);

};

}