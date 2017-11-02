#!/bin/bash

#Usage: ./generate_2d.sh [num entries]

MIN=000
MAX=200000
SEP=0
MAXMASS=50
SIZE=5
RADIUS=8

rm 2d_gen

echo "2 $1" >> 2d_gen

for (( i=0; i < $1 ; i++)) ; do
  
  if (( $((RANDOM % 2)) == 0 )) ; then
    X=$((RANDOM % MAX))
    X=$((X + SEP))
    Y=$((RANDOM % MAX))
    Y=$((Y + SEP))
  else
    X=$((RANDOM % MAX))
    Y=$((RANDOM % MAX))

  fi
  MASS=$((RANDOM % MAXMASS))
  MASS=$((MASS + 1))

  printf "%f\t%f\t%f\t%f\t%f\t%f\t%d\n" $X $Y 0 0 $MASS $RADIUS $i >> 2d_gen
done
