#pragma once

#include <vector>
#include <string>
#include <map>
#include <random>
#include "database.hpp"
#include "exact_search.hpp"

namespace image_search {

//Καθε struct LSHFunction ειναι το τυχαιο διανυσμα μας (v) πανω στο οποιο προβάλλουμε την καθε BoVW εικονα του database μας (δηλαδη το διανυσμα x)
//Το αποτελεσμα της προβολής (x*v) δηλαδη το εσωτερικό γινόμενο είναι ενας αριθμός. Αυτο τον αριθμό τον προσθέτουμε με την τυχαια μετατοπιση t
// Ετσι εχουμε εναν αριθμο x*v + t . Αυτον τον αριθμο τον διαιρουμε με το w (struct LSHFamily) και παίρνουμε σαν αποτελεσμα το bucket του hash table (h_i) στο οποιο ανήκει
// Ομως εχουμε k συναρτησεις κατακερματισμου αρα θα γινει η παραπανω διαδιακσια k φορες αρα θα εχω k συνολικα h_i.
//Ομως εχουμε L συνολικα hash tables. Αρα την παραπνω διαδικασια θα την κανουμε L φορες και τους L 

struct LSHFunction{
    std::vector<float> v; //τυχαίο διάνυσμα προβολής. Ειναι η κατευθυνση πανω στην οποια προβαλλουμε το διανυσμα x
    float t; //τυχαία μετατόπιση
};

//Δομη η οποια κραταει μια ομαδα απο συναρτησεις hashing
struct LSHFamily{
    std::vector<LSHFunction> functions; // Vector το οποιο περιεχει όλες τις τυχαιες ευθειες πληθους k
    int k; // Μας δειχνει ποσες LSHFunction περιεχει η δομη
    float w; // Σε ποσα τμηματα διαιρουμε την προβολή

    //Η συνάρτηση αυτή παίρνει το διάνυσμα μιας εικόνας, υπολογίζει το αποτέλεσμα κάθε συνάρτησης, Συνδυάζει τα αποτελέσματα και επιστρέφει έναν ενιαίο unsigned int.
    unsigned int computeHash(const std::vector<float>& x) const;

};
class LSHSearch {
public:
    LSHSearch(const Database& db, int k, int L, float w, int seed);


private:
    const Database& db_; // Βαση με τις εικονες σε μορφη BoVW
    int k_; //πόσες συναρτήσεις προβολής θα συνδυάζουμε σε κάθε hash table.
    int L_; // Ποσα hash tables εχουμε συνολικα 
    float w_;// Σε ποσα τμηματα διαιρουμε την προβολή

    //βοηθητικη συναρτηση για ευρεση ευκλειδιας αποστασης
    static float squaredDistance(const std::vector<float>& a, const std::vector<float>& b);
};

}