#!/bin/bash

rm output/*

./bin/n-body_sim -i ./input/2d_gen -o ./output/sample_2d -n 200 -t 40000000 -c
#./bin/n-body_sim -i ./input/2d_solar_system  -o ./output/sample_2d  -n 365 -t 86400
