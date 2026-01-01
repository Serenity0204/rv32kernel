#!/bin/bash

rm -rf build
rm -f programs/*.bin
rm -f programs/*.o
rm lib/*.o
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)