#!/bin/bash
test -v main.o && rm main.o
g++ -fopenmp main.cpp -o main.o -w
./main.o
