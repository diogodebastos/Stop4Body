#!/bin/bash

INPUT=~cbeiraod/local-area/Stop4Body/nTuples_v2016-08-31/
OUTPUT=~cbeiraod/local-area/Stop4Body/nTuples_v2016-08-31_bdt/

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi

  # SET9001
  #applyMVA --json samples.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT} --mvaSuffix NodeSize0.5
  # SET9002
  #applyMVA --json samples.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT} --mvaSuffix NodeSize1
  # SET9003
  #applyMVA --json samples.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT} --mvaSuffix NodeSize1.5
  # SET9004
  #applyMVA --json samples.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT} --mvaSuffix NodeSize2
  # SET9005
  #applyMVA --json samples.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT} --mvaSuffix NodeSize2.5
  # SET9006
  #applyMVA --json samples.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT} --mvaSuffix NodeSize1.5

  applyMVA --json samples.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT}
fi

