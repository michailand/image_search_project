#Ορίζουμε την ελάχιστη εκδοση για συμβατότητα
cmake_minimum_required(VERSION 3.20)
project(image_search_project LANGUAGES CXX)

#Επιβάλλει τη χρήση του προτύπου C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE\_CXX\_STANDARD\_REQUIRED ON)


#Εντοπισμός OpenCV
find_package(OpenCV REQUIRED)

#Δηλώνουμε που θα βρει 