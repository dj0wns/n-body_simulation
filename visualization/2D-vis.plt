#!/bin/gnuplot

#Usage: ./2D-vis.plt <Title> <Input File> <Output File>  

set terminal png size 1000,800 enhanced

set output ARG3

set title ARG1
set xlabel "X Axis (Meters)"
set ylabel "Y Axis (Meters)"

set logscale cb
set cbrange [10000000000000000000000.:1000000000000000000000000000.]
set xrange [-1000000000000.:1000000000000.]
set yrange [-1000000000000.:1000000000000.]

#plot ARG2 using 1:2:5 with points pt 6 ps 0.5 lt palette title ARG2
plot ARG2 using 1:2:5 with points pt 6 ps 0.5 lt palette title ARG2

