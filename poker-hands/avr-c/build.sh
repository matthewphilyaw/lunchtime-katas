#!/bin/zsh

proj_root=$PWD

cd ~/code/builds/poker_hands
make $1
cd $proj_root
