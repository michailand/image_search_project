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

    //Συναρτηση με την οποία θα αποθηκεύουμε το λεξικό σε ενα αρχειο ώστε να μην χρειαζεται να την τρεχουμε καθε φορα
    bool saveVocabulary(const std::string& filename) const;
    //Συνάρτηση για φορτωμα του λεξικου από το αρχείο στο οποίο το εχουμε αποθηκευσει

    //Συνάρτηση για φορτωμα του λεξικου απο το αρχειο βinary στο οποιο το εχουμε αποθηκευσει
    bool loadVocabulary(const std::string& filename);

    //getter για το λεξικό
    const cv::Mat& getVocabulary() const{
        return vocabulary_;
    }

private:
    //Πίνακας στον οποίο θα αποθηκεύσουμε τα KMeans centroids που θα αποτελέσουν το λεξικό μας. Δηλαδη εδω μαζευονται τα centroids απο το σετ των εικονων για train. Αυτος ο πινακας θα χρησιμοποιηθεί για να μετατρέψουμε τα SIFT Descriptors σε διάνυσμα KClusters διαστάσεων για κάθε εικόνα.
    cv::Mat vocabulary_;

    //Παιρνει ως ορίσματα δυο διανυσματα (a & b) (vector) και το μέγεθος τους (dims) δηλαδ�� τις διαστάσεις του και επιστρέφει την απόσταση μεταξύ τους 
    static float squaredDistance(const float* a,const float* b, int dims);



};


}