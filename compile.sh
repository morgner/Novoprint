#! /usr/bin/env bash

SD=`pwd`
SD=${SD##/*/}
EN="NovoPrint-$SD"

export LD_LIBRARY_PATH="/usr/lib/boost"

#clang `pkg-config --libs --cflags gtkmm-3.0 cairomm-1.0` -O3 -std=c++2a -lstdc++ -lm *.cpp -o "$EN"
#clang                                                    -O3 -std=c++2a -lstdc++ -lm -lserialport *.cpp -o "$EN"
clang                                                   -O3 -std=c++2a -lstdc++ -lm -lboost_system -lpthread *.cpp -o "$EN" -L /usr/lib64/

if [[ $? -eq 0 ]]
    then
    ./$EN
    fi

echo "Executable Name: $EN"

