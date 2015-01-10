#!/bin/bash

g++ -I/usr/local/include/raptor2 -I/usr/include/boost -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MFsrc/rete.d -MTsrc/rete.d -o src/rete.o src/rete.cpp && \
g++ -L/usr/local/lib -L/usr/include/boost -o rete  ./src/rete.o -lraptor2 -lboost_regex && \
time ./rete
