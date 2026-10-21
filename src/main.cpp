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


    //---- Δημιουργια της βασης δεδομενων  ----
    std::cout << "Εναρξη δημιουργίας της βασης δεδομένων \n";
    image_search::Database db;
    std::string dbCacheFile ="tests/database_" + cfg.querySet + "_D"+ std::to_string(cfg.D) + "_K" +std::to_string(cfg.K) + ".bin";
    bool dbCreated= db.loadOrBuild(cfg.hpatchesDir,cfg.mirflickrDir,split,targetSetType,cfg.D,bovw, dbCacheFile);

    if(dbCreated){
        std::cout << "Η βάση είναι έτοιμη με " << db.size() << " αντικείμενα.\n\n";
    }
    else{
        std::cout<< "ΠΡΟΣΟΧΗ!!! Η βαση δεν φορτωθηκε \n";
        return 1;
    }

    //--- Εκτέλεση της επιλεγμένης από τον χρηστη μεθόδου


    return 0;
}