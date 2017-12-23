#!/bin/bash

#Usage: ./generate_2d.sh [num entries]

MIN=000
MAX=600000000
SEP=1000000
MAXMASS=5000
ADDEDPOS=000
ADDEDMASS=00000000000000000000
SIZE=5
RADIUS=800000000

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
  else
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
 # MASS=$((MAXMASS))

  printf "%d%s.0\t%d%s.0\t%f\t%f\t%d%s.0\t%f\t%d\n" $X $ADDEDPOS $Y $ADDEDPOS 0 0 $MASS $ADDEDMASS $RADIUS $i >> 2d_gen
done
