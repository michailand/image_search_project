#pragma once

#include <string>
#include <vector>
#include "exact_search.hpp"

namespace image_search{



    //Δομή για να κρατάμε τις μετρικές ενός μεμονωμένου query
    struct QueryMetrics {
        float recall5;
        float recall10;
        float ap10;
    };

    class Metrics {
        public:

        // Υπολογίζει τις μετρικές για ένα συγκεκριμένο query.
        //querySeqName: το όνομα της ακολουθίας (π.χ. "i_ajuntament") για να ελέγξουμε τις σωστές απαντήσεις
        static QueryMetrics evaluateQuery(const std::string& querySeqName, const std::vector<SearchResult>& results);
    };

}