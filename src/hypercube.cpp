#include "hypercube.hpp"
#include <iostream>
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace image_search {

float HypercubeSearch::squaredDistance(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0.0f;
    size_t dim = std::min(a.size(), b.size());
    for (size_t i= 0;i<dim;i++) {
        float diff= a[i] - b[i];
        sum += diff * diff;
    }
    return sum;
}

//Συναρτηση με την οποία βρίσκουμε σε ποια κορυφη ανηκει μια εικονα στη μορφη διανυσματος BoVW
uint32_t HypercubeSearch::computeVertex(const std::vector<float>& x) const {
    uint32_t vertex = 0;
    if (x.empty()) return 0; 

    for (int i =0;i< kproj_;i++) {
        float dot_product = 0.0f;
        
        size_t dim= std::min(x.size(), functions_[i].v.size());
        for (size_t j = 0;j< dim;j++) {
            dot_product += x[j] * functions_[i].v[j];
        }

        int h_i = static_cast<int>(std::floor((dot_product + functions_[i].t)/ w_));

        uint32_t mixed = static_cast<uint32_t>(h_i) ^ functions_[i].salt;
        mixed ^= (mixed >> 16);
        mixed *= 0x85ebca6b;
        mixed ^= (mixed >> 13);
        uint32_t bit = mixed & 1u;

        vertex = (vertex << 1) | bit;
    }

    return vertex;
}



// vertex: Η αρχική κορυφή, συνήθως η κορυφή του ερωτήματος.
// kproj: Το πλήθος των bits που χρησιμοποιούνται για την αναπαράσταση μιας κορυφής.
// dist: Η επιθυμητή απόσταση Hamming από την αρχική κορυφή. Μετρά πόσα bits διαφέρουν.
// maxProbes: Το μέγιστο πλήθος κορυφών που επιτρέπεται να προστεθούν στο αποτέλεσμα.
// outVertices: Το διάνυσμα στο οποίο αποθηκεύονται οι κορυφές που επιλέγονται για εξέταση.

void HypercubeSearch::generateHammingNeighbors(uint32_t vertex, int kproj, int dist, size_t maxProbes, std::vector<uint32_t>& outVertices) {
    if (outVertices.size()>= maxProbes) return;

    if (dist== 0) {
        outVertices.push_back(vertex);
        return;
    }

    uint32_t maxVal = 1u << kproj;
    for (uint32_t v = 0; v< maxVal;v++) {
        if (outVertices.size()>= maxProbes) return;
        
        uint32_t xor_val = vertex ^ v;
        int current_dist = 0;
        uint32_t temp = xor_val;
        
        while (temp > 0) {
            current_dist += (temp & 1);
            temp >>= 1;
        }
        if (current_dist== dist) {
            outVertices.push_back(v);
        }
    }
}

// kproj: Το πλήθος των bits που θα σχηματίζουν το ID κάθε κορυφής.
// w: Το πλάτος των διαστημάτων της συνάρτησης κατακερματισμού.
// M: Το μέγιστο πλήθος σημείων της βάσης που επιτρέπεται να εξεταστούν κατά την αναζήτηση.
// probes: Το μέγιστο πλήθος κορυφών που επιτρέπεται να εξεταστούν.
HypercubeSearch::HypercubeSearch(const Database& db, int kproj, float w, int M, int probes, int seed)
    : db_(db), kproj_(kproj), w_(w), M_(M), probes_(probes) {

    const auto& items = db_.getItems();
    if (items.empty()) return;

    size_t dim = items[0].vec.size();

    std::mt19937 gen(seed);
    std::normal_distribution<float> d_normal(0.0f, 1.0f);
    std::uniform_real_distribution<float> d_uniform(0.0f, w_);
    std::uniform_int_distribution<uint32_t> d_salt(1u, 0xFFFFFFFFu);

    functions_.resize(kproj_);
    for (int i =0; i <kproj_;i++) {
        functions_[i].v.resize(dim);
        for (size_t j = 0; j < dim;j++) {
            functions_[i].v[j] = d_normal(gen);
        }
        functions_[i].t = d_uniform(gen);
        functions_[i].salt = d_salt(gen);
    }

    std::cout << "[Hypercube] Δημιουργία Υπερκύβου με kproj=" << kproj_ << ", w=" << w_   << ", M=" << M_ << ", probes=" << probes_ << "\n";

    for (const auto& item :items) {
        if (!item.vec.empty()) {
            uint32_t vertex = computeVertex(item.vec);
            hypercube_table_[vertex].push_back(item);
        }
    }

    std::cout << "[Hypercube] Εισήχθησαν " << db_.size() << " διανύσματα σε " << hypercube_table_.size() << " ενεργές κορυφές.\n";
}

std::vector<SearchResult> HypercubeSearch::search(const std::vector<float>& queryVec, int topK) const {
    std::vector<SearchResult> results;
    
    if (queryVec.empty()) {
        return results;
    }

    std::unordered_set<std::string> seenIds;
    uint32_t queryVertex = computeVertex(queryVec);

    std::vector<uint32_t> verticesToProbe;
    verticesToProbe.reserve(probes_);

    // ΑΦΑΙΡΕΣΑ το 0 (startBit) από την κλήση εδώ:
    for (int dist = 0; dist <= kproj_ && verticesToProbe.size() < static_cast<size_t>(probes_); ++dist) {
        generateHammingNeighbors(queryVertex, kproj_, dist, static_cast<size_t>(probes_), verticesToProbe);
    }

    int checkedPoints = 0;

    for (uint32_t v : verticesToProbe) {
        auto it = hypercube_table_.find(v);
        if (it != hypercube_table_.end()) {
            for (const auto& item : it->second) {
                if (seenIds.find(item.id) == seenIds.end()) {
                    seenIds.insert(item.id);
                    float dist = squaredDistance(queryVec, item.vec);
                    results.push_back({item.id, dist});
                    checkedPoints++;
                }

                if (checkedPoints >= M_) break;
            }
        }
        if (checkedPoints >= M_) break;
    }

    if (results.size() < static_cast<size_t>(topK)) {
        const auto& items = db_.getItems();
        for (const auto& item : items) {
            if (!item.vec.empty() && seenIds.find(item.id) == seenIds.end()) {
                seenIds.insert(item.id);
                float dist = squaredDistance(queryVec, item.vec);
                results.push_back({item.id, dist});
            }
            if (results.size() >= static_cast<size_t>(topK)) break;
        }
    }

    size_t k = std::min(static_cast<size_t>(topK), results.size());
    if (k > 0) {
        std::partial_sort(results.begin(), results.begin() + k, results.end(),
                          [](const SearchResult& a, const SearchResult& b) {
                              return a.distance < b.distance;
                          });
        results.resize(k);
    }

    return results;
}

} // namespace image_search