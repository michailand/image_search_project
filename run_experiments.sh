#!/bin/bash

# Κάνουμε compile για σιγουριά
echo "--- Compiling Project ---"
cmake --build build -j
echo ""

# Ορίζουμε τα μεγέθη της βάσης D
D_VALUES=(1000 5000 10000 25000)

# Ορίζουμε το αρχείο όπου θα μαζευτούν ΟΛΑ τα αποτελέσματα
FINAL_OUTPUT="ALL_RESULTS.txt"
> $FINAL_OUTPUT # Αδειάζει το αρχείο αν υπάρχει ήδη

# Βασικές παράμετροι
BASE_ARGS="-hp data/hpatches-sequences-release -mir data/mirflickr -split split.txt -vocab 256 -S 500 -set test -seed 1"

echo "=========================================="
echo " Ξεκινάει η Πειραματική Μελέτη (Test Set) "
echo "=========================================="

for D in "${D_VALUES[@]}"; do
    echo "==========================================" | tee -a $FINAL_OUTPUT
    echo " ΕΚΤΕΛΕΣΕΙΣ ΓΙΑ D = $D " | tee -a $FINAL_OUTPUT
    echo "==========================================" | tee -a $FINAL_OUTPUT

    # 1. Exact Search
    echo ">>> Running Exact Search for D=$D..."
    ./build/image_search_project $BASE_ARGS -exact -D $D -o exact_d${D}.txt
    tail -n 6 exact_d${D}.txt >> $FINAL_OUTPUT # Παίρνει μόνο τα συνολικά στατιστικά στο τέλος

    # 2. LSH (k=4, L=15, w=0.1)
    echo ">>> Running LSH for D=$D..."
    ./build/image_search_project $BASE_ARGS -lsh -k 4 -L 15 -w 0.1 -D $D -o lsh_d${D}.txt
    tail -n 6 lsh_d${D}.txt >> $FINAL_OUTPUT

    # 3. Hypercube (kproj=10, M=500, probes=100, w=0.1)
    echo ">>> Running Hypercube for D=$D..."
    ./build/image_search_project $BASE_ARGS -hypercube -kproj 10 -w 0.1 -M 500 -probes 100 -D $D -o hc_d${D}.txt
    tail -n 6 hc_d${D}.txt >> $FINAL_OUTPUT

    # 4. IVFFlat (nprobe=10)
    echo ">>> Running IVFFlat for D=$D..."
    ./build/image_search_project $BASE_ARGS -ivfflat -kclusters 0 -nprobe 10 -D $D -o ivfflat_d${D}.txt
    tail -n 6 ivfflat_d${D}.txt >> $FINAL_OUTPUT

    # 5. IVFPQ (nprobe=10, M=16, nbits=8)
    echo ">>> Running IVFPQ for D=$D..."
    ./build/image_search_project $BASE_ARGS -ivfpq -kclusters 0 -nprobe 10 -M 16 -nbits 8 -D $D -o ivfpq_d${D}.txt
    tail -n 6 ivfpq_d${D}.txt >> $FINAL_OUTPUT
    
    echo "" | tee -a $FINAL_OUTPUT
done

echo "✅ Όλα τα πειράματα ολοκληρώθηκαν!"
echo "📄 Τα συνοπτικά στατιστικά βρίσκονται στο αρχείο: $FINAL_OUTPUT"
echo "📄 Τα αναλυτικά αποτελέσματα (Rank-1 έως Rank-10) βρίσκονται στα επιμέρους .txt αρχεία."