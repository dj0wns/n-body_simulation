#!/bin/gnuplot

#Usage: ./2D-vis.plt <Title> <Input File> <Output File>  

set terminal png size 1000,800 enhanced

set output ARG3

set title ARG1
set xlabel "X Axis (Meters)"
set ylabel "Y Axis (Meters)"

set xrange [-8000000:8000000]
set yrange [-8000000:8000000]

#plot for [i=0:ARG1] file=sprintf("../output/sample_2d_%04d.dat",i) file using 1:2 with points
plot ARG2 using 1:2 with points title ARG2

