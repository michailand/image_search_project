#pragma once

#include <vector>
#include <string>
#include <map>
#include "database.hpp"
#include "kmeans.hpp"
#include "exact_search.hpp"



namespace image_search {

class IVFFlat {
    public:


    private:

        //Εδω θα κραταμε την βαση δεδομενων με τις εικονες σε BovW μορφη, το πλήθος των clusters   που θελουμε να φτιαξουμε, τα ίδια τα clusters που φτιάξαμε
        const Database& db_;
        int kclusters_;
        cv::Mat centroid_;// Τα κέντρα του K-Means της βάσης δηλαδη τα clusters μας.


        //Εδω αποθηκευεται ο map ο οποιος θα αντιστοιχιζει καθε cluster με ενα vector το οποιο vector θα περιλαμβανει ολα τα DatabaseItem(εικονες) που ειναι κοντα του
        std::map<int, std::vector<DatabaseItem>> index_;

        //Synartisi gia ton ypologismo ths eykleidias apostasis
        static float squareDistance(const std::vector<float>& a,const std::vector<float>& b);
        
};

}