#!/bin/zsh

proj_root=$PWD
tool_chain=$proj_root/toolchain-avr-gcc.cmake

cd build
cmake -DCMAKE_TOOLCHAIN_FILE=$tool_chain $proj_root
cd ..
