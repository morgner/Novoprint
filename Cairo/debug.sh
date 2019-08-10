#! /usr/bin/env bash

SD=`pwd`
SD=${SD##/*/}
EN="CairoGUI-$SD"

clang `pkg-config --libs --cflags gtkmm-3.0 cairomm-1.0` -g -std=c++2a -lstdc++ -lm *.cpp -o "$EN"

if [[ $? -eq 0 ]]
    then
#   nemiver ./$EN
    ddd ./$EN
    fi

echo "Executable Name: $EN"
