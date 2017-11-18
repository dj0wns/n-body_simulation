#!/bin/bash
TITLE=TEST
OUTDIR=plt
INDIR=../output
INPUT=
OUTPUT=${OUTDIR}/sample.eps

ALPHAEXISTS=false

rm ${OUTDIR}/*
rm animated.gif

for f in ${INDIR}/*; do
  OUT=$(basename $f)
  gnuplot -c 2D-vis.plt ${TITLE} ${f} ${OUTDIR}/${OUT%.*}.png
done
  ffmpeg -i ${OUTDIR}/sample_2d_%04d.png animated.gif
