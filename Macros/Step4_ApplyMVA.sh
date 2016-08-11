#!/bin/bash

INPUT=~/local-area/Stop4Body/New/
OUTPUT=~/local-area/Stop4Body/nTuples_v2016-07-23_bdt/
INPUT=~/local-area/Stop4Body/LepFix/

OUTPUT=~/local-area/Stop4Body/NodeSizeScan/SET9007/

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi

  # SET9001
  #applyMVA --json samples4MVA.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT} --suffix NodeSize0.5
  # SET9002
  #applyMVA --json samples4MVA.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT} --suffix NodeSize1
  # SET9003
  #applyMVA --json samples4MVA.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT} --suffix NodeSize1.5
  # SET9004
  #applyMVA --json samples4MVA.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT} --suffix NodeSize2
  # SET9005
  #applyMVA --json samples4MVA.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT} --suffix NodeSize2.5

  # SET9006
  #applyMVA --json samples4MVA.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT} --suffix NodeSize1.5
  # SET9006
  applyMVA --json samples4MVA.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT}
fi

