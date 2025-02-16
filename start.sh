#!/bin/bash

clear

root="./code"

g++ -o $root/main.out $root/main.cpp -std=c++17

if [ $? -eq 0 ]; then
    $root/main.out
else
    echo "Erro na compilação"
fi
