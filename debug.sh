#! /usr/bin/env bash

SD=`pwd`
SD=${SD##/*/}
EN="NovoPrint-$SD"

#clang `pkg-config --libs --cflags gtkmm-3.0 cairomm-1.0` -O3 -std=c++2a -lstdc++ -lm *.cpp -o "$EN"
clang -g -std=c++2a -lstdc++ -lm *.cpp -o "$EN"

if [[ $? -eq 0 ]]
    then
    nemiver ./$EN
    fi

echo "Executable Name: $EN"

