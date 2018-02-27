#!/bin/bash

rm output/*

time mpirun -np 4  ./bin/n-body_sim -i ./input/2d_gen -o ./output/sample_2d -O 10 -n 2000 -t 400000 
#./bin/n-body_sim -i ./input/2d_solar_system  -o ./output/sample_2d  -n 365 -t 86400 -c 
