#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>


namespace image_search{
class BoVW {
public:
    //Συνάρτηση για την δημιουργία του λεξικού
    bool buildVocabulary(const std::string& hpatchesDir,const std::vector<std::string>& trainSequences,int KClusters,int maxDescriptorPerImage=500,unsigned int seed=1,int maxIters=30);

    //Μετατροπή των SIFT Descriptors σε διάνυσμα διάστασης KClusters. Ετσι καθε εικόνα θα αναπαρίσταται από ένα διάνυσμα KClusters διαστάσεων και θα μπορει να γινει ευκολα η αναζήτηση εικόνων με βάση την ομοιότητα των διανυσμάτων τους
    std::vector<float> computeVector(const cv::Mat& descriptors) const;


private:
    //Πίνακας στον οποίο θα αποθηκεύσουμε τα KMeans centroids που θα αποτελέσουν το λεξικό μας. Δηλαδη εδω μαζευονται τα centroids απο το σετ των εικονων για train. Αυτος ο πινακας θα χρησιμοποιηθεί για να μετατρέψουμε τα SIFT Descriptors σε διάνυσμα KClusters διαστάσεων για κάθε εικόνα.
    cv::Mat vocabulary_;
};



}