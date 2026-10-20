#include <iostream>
#include <opencv2/opencv.hpp>
#include "split.hpp"
#include "sift_extractor.hpp"
#include "kmeans.hpp"
#include "BoVW.hpp"
#include <cmath>

#include "database.hpp"
#include <algorithm>
#include "exact_search.hpp"
#include "metrics.hpp"
#include <chrono>


// Προκειμένου να δεχομαστε ορισματα με χρηση flags απο την main θα φτιαξουμε μια συνάρτηση η οποια
// θα διαβαζει την CLI και θα αποθηκευει σε ενα struct αυτα τα ορισματα

struct Config{
    //Θα δώσουμε και default τιμές στις παραμέτρους
    std::string hpatchesDir = "data/hpatches-sequences-release";  //kατάλογος του HPatches.
    std::string mirflickrDir="data/mirflickr"; // kατάλογος του MIRFlickr-25K.
    std::string splitFile= "split.txt"; // αρχείο με τον διαχωρισμό training, validation kαι test.

    int K=100;  // -vocab αριϑμός K των kέντρων του kmeans των SIFT descriptors.
    int S=500; //-S μέγιστο πλήϑος SIFT descriptors ανά training image για την εkπαίδευση του k-means.
    int D=1000; //-D πλήϑος ειkόνων MIRFlickr

    std::string querySet="validation"; //-set σύνολο queries.
    std::string outputFile="output.txt"; //-o αρχείο εξόδου.

    int seed=1; //-seed φύτρο ψευδοτυχαίων αριϑμών.

    //Εδώ θα βαζουμε την μέθοδο που ζήτησε ο χρήστης
    std::string method ="exact"; //Η μέϑοδος αναζήτησης επιλέγεται με μία από τις παραμέτρους
                                //-exact -lsh -hypercube -ivfflat -ivfpq

    //LSH flags
    int lsh_k=4; //πλήϑος των συναρτήσεων hi που συνϑέτουν kάϑε συνάρτηση kαταkερματισμού g. 
    int lsh_L=5; // πλήϑος των πινάkων kαταkερματισμού.
    double lsh_w=0.05; //μήkος του kελιού επί της ευϑείας προβολής.

    //Hypercube flags
    int hc_kproj = 10; //διάσταση d′του υπερkύβου στον οποίο προβάλλονται τα σημεία.
    double hc_w = 0.05; //μήkος του kελιού επί της ευϑείας προβολής. 
    int hc_M = 500; //μέγιστο πλήϑος υποψήφιων σημείων που ελέγχονται.
    int hc_probes = 10;// μέγιστο πλήϑος kορυφών του υπερkύβου που ελέγχονται.

    // IVFFlat & IVFPQ Flags
    int ivf_kclusters = 0; //πλήϑος συστάδων του ευρετηρίου.  το "0" σημαίνει υπολόγισε το ceil(sqrt(n))
    int ivf_nprobe = 5; // πλήϑος των πλησιέστερων συστάδων που ελέγχονται kατά την αναζήτηση.
    
    //IVFPQ Flags
    int pq_M = 16; //πλήϑος υποδιανυσμάτων του Product Quantization. Το K πρέπει να διαιρείται αkριβώς με το M. 
    int pq_nbits = 8; //kάϑε υποχώρος συσταδοποιείται σε 2 nbits συστάδες.

};

// Εδω θα υλοποιηθεί ο Parser της CLI
Config parseArgs(int argc, char** argv){
    //Εδω θα αποθηκευτεί το output
    Config cfg;

    for(int i=0;i<argc;i++){
        std::string arg=argv[i];

        //Πάντα θα ελέγχουμε αν το flag που εξετάζουμε ειναι το τελευταιο. Γιατι? Για να αποφυγουμε seg fault οταν παμε να διαβασουμε/ελέγξουμε αν ακολουθει μετά το flag τιμή 
        if( (arg == "-hp") && (i+1<argc) ) cfg.hpatchesDir = argv[++i];
        else if( (arg == "-mir") && (i+1<argc) ) cfg.mirflickrDir = argv[++i];
        else if( (arg == "-split") && (i+1<argc) ) cfg.splitFile = argv[++i];
        else if( (arg == "-vocab") && (i+1<argc) ) cfg.K = std::stoi(argv[++i]);
        else if( (arg == "-S") && (i+1<argc) ) cfg.S = std::stoi(argv[++i]);
        else if( (arg == "-D") && (i+1<argc) ) cfg.D = std::stoi(argv[++i]);
        else if( (arg == "-set") && (i+1<argc) ) cfg.querySet = argv[++i];
        else if( (arg == "-o") && (i+1<argc) ) cfg.outputFile = argv[++i];
        else if( (arg == "-seed") && (i+1<argc) ) cfg.seed =std::stoi(argv[++i]);

        //Αποθηκευετε η μεθοδος αναζητησης
        else if(arg=="-exact") cfg.method= "exact";
        else if(arg=="-ivfflat") cfg.method= "ivfflat";
        else if(arg=="-ivfpq") cfg.method= "ivfpq";
        else if(arg=="-lsh") cfg.method= "lsh";
        else if(arg=="-hypercube") cfg.method= "hypercube";

        //Αποθηκεύονται οι παραμετροι των μεθόδων
        //LSH
        else if((arg=="-k") && (i+1<argc)) cfg.lsh_k= std::stoi(argv[++i]);
        else if((arg=="-L") && (i+1<argc)) cfg.lsh_L= std::stoi(argv[++i]);

        else if((arg == "-w") && (i+ 1 < argc)){
            cfg.lsh_w = std::stod(argv[i+1]);
            cfg.hc_w = std::stod(argv[++i]);
        }
        else if((arg == "-M") && (i+ 1 < argc)){
            cfg.hc_M = std::stoi(argv[i+1]);
            cfg.pq_M = std::stoi(argv[++i]);
        }

        //hypercube
        else if((arg == "-kproj") && (i+ 1 < argc)) cfg.hc_kproj=std::stoi(argv[++i]);
        else if((arg == "-probes") && (i+ 1 < argc)) cfg.hc_probes=std::stoi(argv[++i]);

        //IVFFlat & IVFPQ
        else if (arg == "-kclusters" && i +1 < argc) cfg.ivf_kclusters = std::stoi(argv[++i]);
        else if (arg == "-nprobe" && i +1 < argc) cfg.ivf_nprobe = std::stoi(argv[++i]);
        else if (arg == "-nbits" && i +1 < argc) cfg.pq_nbits = std::stoi(argv[++i]);


    }
    
    return cfg;
}






int main(int argc, char** argv){
    //Κλήση Parser για την συλλογη των flags απο CLI
    std::cout<< "Κλήση του CLI Parser";
    Config cfg= parseArgs(argc,argv);
    std::cout<< "Λήξη του CLI Parser";

    std::cout << CV_VERSION << std::endl;

    std::cout << "Εναρξη image_search project\n";
    std::cout << "Ακολουθεί η παράθεση των παραμέτρων και των μεθόδων που ορίστηκαν από τον χρήστη\n";

    std::cout << "Μέθοδος: " << cfg.method << " | QuerySet: " << cfg.querySet << "\n";
    std::cout << "Παράμετροι: Vocab(K)=" << cfg.K << " | S=" << cfg.S << " | D=" << cfg.D << "\n\n";

    std::cout << "Κλήση μεθόδου για το διάβασμα του αρχείου split\n";

    //Φορτωνουμε το split.txt και αποθηκευουμε τα sequences σε vector αναλογα τον τύπο (SetType) της ακολουθίας
    image_search::DatasetSplit split;
    if(!split.loadFromFile(cfg.splitFile)){
        std::cerr << "Failed to load "<< cfg.splitFile <<" file \n";
        return 1;
    }
    
    auto train=split.getSequences(image_search::SetType::TRAIN);
    auto val=split.getSequences(image_search::SetType::VALIDATION);
    auto test=split.getSequences(image_search::SetType::TEST);

    std::cout << " Φορτωμα του Split \n";
    //Αποθηκευουμε την τιμη που δοθηκε στο -set ωστε να παρουμε ολες τις αντιστοιχες ακολουθιες 
    auto targetSetType= (cfg.querySet=="test")? image_search::SetType::TEST : image_search::SetType::VALIDATION;
    auto targetSet= split.getSequences(targetSetType);



    // ---Φορτωση Λεξιλογιου BoVW ---
    std::cout<< "---- Εναρξη δημιουργιας λεξιλογιου ----\n";
    image_search::BoVW bovw;

    std::string vocabFile="tests/vocabulary_K"+ std::to_string(cfg.K) + "_S" + std::to_string(cfg.S) + ".bin";
    if(!bovw.loadVocabulary(vocabFile)){
        std::cout<< "Το λεξικο δεν υπάρχει και ξεκινα η εκπαίδευση του νεου λεξικού \n";
        bool res= bovw.buildVocabulary(cfg.hpatchesDir,train,cfg.K,cfg.S,cfg.seed,30);
        if (res) {
            std::cout<< "Το λεξικό δημιουργήθηκε με επιτυχία\n";
            std::cout<< "Το λεξικό θα αποθηκευτεί στο filesystem\n";
            bool res1= bovw.saveVocabulary(vocabFile);
            if(res1) std::cout<< "Το λεξικό αποθηκευτηκε στο filesystem\n";
            else std::cout<< "ΠΡΟΣΟΧΗ!!! Το λεξικό ΔΕΝ αποθηκευτηκε στο filesystem\n";
        }
        else {
            std::cout<< "Η δημιουργία του λεξικού απευχε\n";
            return 1;
        }
    }


    const cv::Mat& vocabulary =bovw.getVocabulary();

    std::cout << "To vocabulary ftiaxthke kai exei rows= " << vocabulary.rows << " kai cols= " << vocabulary.cols << "\n";

    //Apothikeysi se arxeio toy vocabulary
    std::string vocabFile= "tests/vocabulary.bin";
    bool storeVocab= bovw.saveVocabulary(vocabFile);
    if(storeVocab){
        std::cout << "To vocabulary apothikeythke sto arxeio: " << vocabFile << "\n";
    }
    else{
        std::cerr << "Failed to save vocabulary to file: " << vocabFile << "\n";
    }

    //Τεστ για να δω αν το loadVocabulary δουλευει σωστα. 
    image_search::BoVW bovw2;
    bool loadVocab2= bovw2.loadVocabulary(vocabFile);
    if(!loadVocab2){
        std::cerr<<"ΠΡΟΣΟΧΗ!!! Το loadVocabulary απέτυχε για το αρχείο: "<<vocabFile<<"\n";
    }
    bool storeVocab2= bovw2.saveVocabulary("tests/vocabulary2.bin");
    if(!storeVocab2){
        std::cerr<<"ΠΡΟΣΟΧΗ!!! Το saveVocabulary απέτυχε για το αρχείο: tests/vocabulary2.bin\n";
    }   

    //Ελεγχος των δυο binary αρχειων
    int res= std::system("cmp tests/vocabulary.bin tests/vocabulary2.bin");
    if(res==0){
        std::cout << "Τα δυο αρχεία vocabulary.bin και vocabulary2.bin είναι ίδια.\n";
    }
    else{
        std::cerr << "ΠΡΟΣΟΧΗ!!! Τα δυο αρχεία vocabulary.bin και vocabulary2.bin ΔΕΝ είναι ίδια.\n";
    }

    std::string sampleImgPath= "data/hpatches-sequences-release/" + train[0] + "/1.ppm";

    std::cout << "Dokimi gia thn eikona: " << sampleImgPath << "\n";

    std::cout<< "exagogi SIFT descriptors gia to sampleImgPath \n";
    cv::Mat sampleDescriptors= image_search::extractSIFTDescriptors(sampleImgPath);

    std::cout << "To sampleDescriptors exei rows= " << sampleDescriptors.rows << " kai cols= " << sampleDescriptors.cols << "\n";


    std::vector<float> imageVector= bovw.computeVector(sampleDescriptors);
    std::cout << "To imageVector exei size= " << imageVector.size() << "\n";

    float sumDims=0.0f;
    std::cout << "To imageVector= [";
    for(std::size_t i=0;i<imageVector.size();i++){
        std::cout << imageVector[i] << " , ";
        sumDims+= imageVector[i];
    }
    std::cout << "]\n";

    std::cout << "To athroisma twn diastasewn tou imageVector= " << sumDims << "\n";

    //Δοκιμη για την δημιουργια της βασης δεδομενων
    std::cout << "Dokimi gia thn dimiourgia ths bashs dedomenwn \n";
    image_search::Database db;

    bool dbCreated= db.loadOrBuild("data/hpatches-sequences-release","data/mirflickr",split,image_search::SetType::VALIDATION,1000,bovw,"tests/database.bin");

    if(dbCreated){
        std::cout << "Η νεα βαση δεδομενων δημιουργηθηκε επιτυχως και αποθηκευτηκε στο αρχειο: tests/database.bin\n";
        
        const auto& items= db.getItems();
        std::cout << "Η νεα βαση δεδομενων περιεχει συνολικα " << items.size() << " αντικείμενα (εικόνες)\n";   
        for (size_t i = 0; i < std::min(size_t(3), items.size()); ++i) {
        std::cout << " Item " << i << " -> ID: " << items[i].id  << " Vector size: " << items[i].vec.size() << "\n";
    }
    }
    else{
        std::cerr << "ΠΡΟΣΟΧΗ!!! Η νεα βαση δεδομενων ΔΕΝ δημιουργηθηκε επιτυχως και ΔΕΝ αποθηκευτηκε στο αρχειο: tests/database.bin\n";
    }

// --- Test Exact Search και Metrics ---
    std::cout << "\n=== Test Exact Search + Metrics ===\n";

    image_search::ExactSearch searcher(db);

    // Συγκεντρωτικές μετρικές για τα validation queries
    double totalRecall5 = 0.0;
    double totalRecall10 = 0.0;
    double totalAP10 = 0.0;
    int evaluatedQueries = 0;

    // Εκτέλεση αναζήτησης για κάθε εικόνα του Validation set
    for (const auto& seq : val) {
        std::string queryPath =
            "data/hpatches-sequences-release/" + seq + "/1.ppm";

        std::cout << "\nQuery sequence: " << seq << "\n";
        std::cout << "Query image: " << queryPath << "\n";

        // Εξαγωγή SIFT descriptors
        cv::Mat descriptors =
            image_search::extractSIFTDescriptors(queryPath);

        if (descriptors.empty()) {
            std::cerr << "Σφάλμα: Δεν εξήχθησαν SIFT descriptors.\n";
            continue;
        }

        // Δημιουργία BoVW διανύσματος για το query
        std::vector<float> queryVec =
            bovw.computeVector(descriptors);

        if (queryVec.empty()) {
            std::cerr << "Σφάλμα: Το query vector είναι κενό.\n";
            continue;
        }

        // Μέτρηση χρόνου αναζήτησης
        auto start = std::chrono::steady_clock::now();

        // Αναζήτηση των 10 κοντινότερων εικόνων
        auto results = searcher.search(queryVec, 10);

        auto end = std::chrono::steady_clock::now();

        std::chrono::duration<double, std::milli> elapsed = end - start;

        // Εκτύπωση αποτελεσμάτων
        std::cout << "Top-" << results.size() << " results:\n";

        for (std::size_t i = 0; i < results.size(); ++i) {
            std::cout << i + 1
                        << ". ID: " << results[i].id
                        << " | Distance: " << results[i].distance
                        << "\n";
        }

        std::cout << "Search time: " << elapsed.count() << " ms\n";

        // --- Υπολογισμός μετρικών για το συγκεκριμένο query ---
        image_search::QueryMetrics metrics =
            image_search::Metrics::evaluateQuery(seq, results);

        std::cout << "Recall@5:  " << metrics.recall5 << "\n";
        std::cout << "Recall@10: " << metrics.recall10 << "\n";
        std::cout << "AP@10:     " << metrics.ap10 << "\n";

        // Συγκέντρωση τιμών για τον τελικό μέσο όρο
        totalRecall5 += metrics.recall5;
        totalRecall10 += metrics.recall10;
        totalAP10 += metrics.ap10;
        evaluatedQueries++;
    }

    // --- Συνολικά αποτελέσματα Validation ---
    std::cout << "\n=== Validation Metrics Summary ===\n";
    std::cout << "Evaluated queries: " << evaluatedQueries << "\n";

    if (evaluatedQueries > 0) {
        std::cout << "Mean Recall@5:  "
                    << totalRecall5 / evaluatedQueries << "\n";

        std::cout << "Mean Recall@10: "
                    << totalRecall10 / evaluatedQueries << "\n";

        std::cout << "Mean AP@10:     "
                    << totalAP10 / evaluatedQueries << "\n";
    } else {
        std::cout << "Δεν αξιολογήθηκε κανένα query.\n";
    }


    return 0;
}