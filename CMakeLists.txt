#ελάχιστη έκδοση CMake για συμβατότητα
cmake_minimum_required(VERSION 3.20)
project(image_search_project LANGUAGES CXX)

# C++20Πρότυπο C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE\_CXX\_STANDARD\_REQUIRED ON)


#Εντοπισμός OpenCV
find_package(OpenCV REQUIRED)

#Δηλώνουμε που βρίσκεται ο φακελος include για τα header αρχεια
include_directories(${CMAKE_SOURCE_DIR}/include)

#Βαζουμε τα Warnings flag
if(NOT MSVC) 
    add_compile_options(-Wall -Wextra -Wpedantic) 
endif()

#Δηλώνουμε το τελικό Εκτελέσιμο & τα source files απο τα οποία αποτελείται
add_executable(image_search_project
    src/main.cpp
    src/split.cpp
    src/sift_extractor.cpp
    src/kmeans.cpp
    src/BoVW.cpp
    src/database.cpp
    src/exact_search.cpp
    src/metrics.cpp
    src/ivf_flat.cpp
    src/ivf_pq.cpp
    src/lsh.cpp
    src/hypercube.cpp
)


#Σύνδεση με OpenCV
target_link_libraries(image_search_project PRIVATE ${OpenCV_LIBS})