#!/bin/bash

#Usage: ./generate_2d.sh [num entries]

MIN=000
MAX=5000000
SEP=0
MAXMASS=500000
SIZE=5
RADIUS=10000

rm 2d_gen

echo "2 $1" >> 2d_gen

for (( i=0; i < $1 ; i++)) ; do
  rand=$((RANDOM % 4))
  if (( $rand == 0 )) ; then
    rand=$(openssl rand 4 | od -DAn)
    X=$((rand % MAX))
    rand=$(openssl rand 4 | od -DAn)
    Y=$((rand % MAX))
  elif (( $rand == 1 )) ; then
    rand=$(openssl rand 4 | od -DAn)
    X=$((rand  % MAX))
    X=$((-X))
    rand=$(openssl rand 4 | od -DAn)
    Y=$((rand  % MAX))
  elif (( $rand == 2 )) ; then
    rand=$(openssl rand 4 | od -DAn)
    X=$((rand  % MAX))
    rand=$(openssl rand 4 | od -DAn)
    Y=$((rand  % MAX))
    Y=$((-Y))
  elif (( $rand == 3 )) ; then
    rand=$(openssl rand 4 | od -DAn)
    X=$((rand  % MAX))
    X=$((-X))
    rand=$(openssl rand 4 | od -DAn)
    Y=$((rand  % MAX))
    Y=$((-Y))
  fi
  rand=$(openssl rand 4 | od -DAn)
  MASS=$((rand  % MAXMASS))
  MASS=$((MASS + 1))

  printf "%f\t%f\t%f\t%f\t%f\t%f\t%d\n" $X $Y 0 0 $MASS $RADIUS $i >> 2d_gen
done
