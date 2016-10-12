#!/bin/bash

INPUT=~/local-area/Stop4Body/nTuples_v2016-09-09/
OUTPUT_PD=~/local-area/Stop4Body/PseudoDataMC/
OUTPUT_Data=~/local-area/Stop4Body/DataMC/

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT_PD} ]] ; then
    mkdir -p ${OUTPUT_PD}
  fi
  if [[ ! -d ${OUTPUT_Data} ]] ; then
    mkdir -p ${OUTPUT_Data}
  fi

  #getYields --json samples2Run.json --inDir ${INPUT} --variables variables.json --outDir ${OUTPUT_PD}
  getYields --json samples2RunData.json --inDir ${INPUT} --variables variables.json --outDir ${OUTPUT_Data} --lumi 1700
fi

