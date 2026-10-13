#include "sift_extractor.hpp"
#include <iostream>
#include <vector>
#include <random>
#include <numeric>
#include <algorithm>

namespace image_search {

    cv::Mat extractSIFTDescriptors(const std::string& imagePath, int maxDescriptors, unsigned int seed){

        //Ξεκινάμε το διάβασμα της εικόνας σε κλίμακα του γκρι
        cv::Mat image=cv::imread(imagePath, cv::IMREAD_GRAYSCALE);

        //Περίπτωση που δεν καταφερε να διαβασει την εικόνα
        if(image.empty()){
            std::cerr << "Σφάλμα κατα το άνοιγμα της εικόνας για την δημιουργία SIFT descriptors \n";
            return cv::Mat();
        }

        //Εξαγωγή SIFT descriptors
        auto sift= cv::SIFT::create();
        std::vector<cv::KeyPoint> keypoints; //Keypoint ειναι το σημείο στην εικόνα που βρεθηκε ο sift descriptor καθως και αλλα στοιχεια
        cv::Mat descriptors; // Εδω αποθηκευονται το διάνυσμα των 128 αριθμων με τα χαρακτηριστικα του descriptor
        sift->detectAndCompute(image, cv::noArray(), keypoints, descriptors); //η βασικη συνάρτηση που παιρνει την εικονα και αποθηκευει στο keypoint & descriptor αυτα που βρήκε

        //Ελέγχουμε αν βρεθηκαν descriptors γιατι μπορεί να τύχει μία εικόνα να ειναι τοσο απλή που να μην εχει κανεναν
        if(descriptors.empty()){
            return cv::Mat();
        }

        //Τσεκάρουμε ότι ο descriptor ειναι τύπου float και αν οχι τοτε τον μετατρεπουμε
        if (descriptors.type() != CV_32F) {
            descriptors.convertTo(descriptors, CV_32F);
        }

        //Ελέγχουμε αν δόθηκε οριο στο maxDescriptors και αν ναι τοτε αν οι descriptors που βρέθηκαν ειναι περισσότεροι του ορίου τότε θα επιλέξω τυχαία maxDescriptors που θα κρατησω
        if (maxDescriptors > 0 && descriptors.rows > maxDescriptors) {
            //Φτιάχνουμε έναν πίνακα με τα indices: 0, 1, ..., N-1. Σκοπός μας ειναι να ανακατέψουμε με random τροπο αυτόν τον πίνακα και όχι τον αρχικό πίνακα με τους descriptors
            std::vector<int> indices(descriptors.rows);
            std::iota(indices.begin(), indices.end(), 0);// αρχικοποιούμε τον πίνακα αρχίζοντας απο το 0 και αυξάνουμε κατα 1

            // Ανακάτεμα με σταθερό seed του πίνακα indices ο οποίος έχει απλά τους δείκτες
            std::mt19937 rng(seed);
            std::shuffle(indices.begin(), indices.end(), rng);

            // Διαβαζουμε τον πίνακα indices ο οποίος εχει τους ανακατεμένους δεικτες και κρατάμε τις πρώτες maxDescriptors γραμμές
            // Πχ στην πρωτη θεση εχει τον δεικτη 308 αρα θα παρω απο τον αρχικο πίνακα τον descriptor που βρισκεται στη θεση 308
            cv::Mat sampledDescriptors(maxDescriptors, descriptors.cols, descriptors.type());
            for (int i = 0; i < maxDescriptors; ++i) {
                descriptors.row(indices[i]).copyTo(sampledDescriptors.row(i));
            }

            return sampledDescriptors;
        }

        return descriptors;
    }
    
}