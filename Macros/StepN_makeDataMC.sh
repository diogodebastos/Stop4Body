#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

INPUT=${NTUPLE_DIR}
OUTPUT=${INPUT}/DataMC-signal-stack
OUTPUT_QUICK=${INPUT}/DataMC_quick

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi
  if [[ ! -d ${OUTPUT_QUICK} ]] ; then
    mkdir -p ${OUTPUT_QUICK}
  fi  

  makePlots --doSummary --quick --json ${JSON_PATH}/plot${YEAR}-inj-quick.json  --outDir ${OUTPUT_QUICK} --inDir ${INPUT} --variables variables2017.json --cuts variables2017.json
  makePlots --doSummary         --json ${JSON_PATH}/plot${YEAR}-inj.json        --outDir ${OUTPUT}       --inDir ${INPUT} --variables variables2017.json --cuts variables2017.json
fi
