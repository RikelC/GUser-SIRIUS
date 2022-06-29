#!/bin/csh
rm -rf CMakeFiles
rm -f CMakeCache.txt cmake_install.cmake libUserLIB.a  libUserLIB.so Makefile
sleep 1
cmake ../
sleep 1
make
