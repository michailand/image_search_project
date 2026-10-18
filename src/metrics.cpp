#include "metrics.hpp"
#include <iostream>

namespace image_search {

QueryMetrics Metrics::evaluateQuery(const std::string& querySeqName, const std::vector<SearchResult>& results) {
    QueryMetrics qm = {0.0f, 0.0f, 0.0f};

    int hits5 = 0;
    int hits10 = 0;
    float sumPrecisions = 0.0f;

    // Οι σχετικές εικόνες είναι πάντα 5 (οι I2...I6 της ίδιας ακολουθίας)
    const int totalRelevant = 5;

    // Το prefix με βάση τον τρόπο που αποθηκεύτηκαν τα IDs στη Database (με underscore)
    const std::string prefix = querySeqName + "_";

    for (std::size_t i = 0; i < results.size() && i < 10; ++i) {
        const std::string& id = results[i].id;
        bool isRelevant = false;

        // Έλεγχος αν το ID ξεκινάει με το prefix (π.χ. "i_ajuntament_")
        if (id.rfind(prefix, 0) == 0) {
            // Απομονώνουμε τον αριθμό της εικόνας στο τέλος
            const std::string imageNumber = id.substr(prefix.size());

            // Ελέγχουμε αν είναι μονοψήφιος αριθμός από το '2' έως το '6'
            if (imageNumber.size() == 1 && imageNumber[0] >= '2' && imageNumber[0] <= '6') {
                isRelevant = true;
            }
        }

        if (isRelevant) {
            if (i < 5) {
                hits5++;
            }
            hits10++;

            // Precision@k = (Σωστά αποτελέσματα μέχρι τη θέση k) / k
            const float precisionAtK = static_cast<float>(hits10) / static_cast<float>(i + 1);
            sumPrecisions += precisionAtK;
        }
    }

    qm.recall5 = static_cast<float>(hits5) / totalRelevant;
    qm.recall10 = static_cast<float>(hits10) / totalRelevant;
    qm.ap10 = sumPrecisions / totalRelevant;

    return qm;
}

} // namespace image_search