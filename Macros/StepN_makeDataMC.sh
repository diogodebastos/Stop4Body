#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

INPUT=${NTUPLE_DIR}
OUTPUT=${INPUT}/DataMC
OUTPUT_POW=${INPUT}/DataMC_pow
OUTPUT_LEP=${INPUT}/DataMC_lep


if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi
  if [[ ! -d ${OUTPUT_POW} ]] ; then
    mkdir -p ${OUTPUT_POW}
  fi
  if [[ ! -d ${OUTPUT_LEP} ]] ; then
    mkdir -p ${OUTPUT_LEP}
  fi
  makePlots --json ${JSON_PATH}/plot${YEAR}.json     --outDir ${OUTPUT}     --inDir ${INPUT} --variables variables.json --cuts variables.json
  #makePlots --json ${JSON_PATH}/plot${YEAR}_pow.json --outDir ${OUTPUT_POW} --inDir ${INPUT} --variables variables.json --cuts variables.json
  #makePlots --json ${JSON_PATH}/plot${YEAR}_lep.json --outDir ${OUTPUT_LEP} --inDir ${INPUT} --variables variables.json --cuts variables.json
fi