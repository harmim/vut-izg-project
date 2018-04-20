#!/bin/sh

mv CMakeLists.txt CMakeLists_mac.txt
mv CMakeLists_original.txt CMakeLists.txt

mkdir -p build
(cd build && cmake -D CMAKE_BUILD_TYPE=DEBUG -D SDL2_DIR=/mnt/matylda1/imilet/installedLibs/lib/cmake/SDL2 .. && make)

mv CMakeLists.txt CMakeLists_original.txt
mv CMakeLists_mac.txt CMakeLists.txt
