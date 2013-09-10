#!/bin/zsh

proj_root=$PWD
tool_chain=$proj_root/toolchain-avr-gcc.cmake

mkdir -p ~/code/builds/poker_hands
cd ~/code/builds/poker_hands
cmake -DCMAKE_TOOLCHAIN_FILE=$tool_chain $proj_root
