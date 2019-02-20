#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

INPUT=${NTUPLE_DIR}
OUTPUT=${INPUT}/DataMC-signal-stack
OUTPUT_QUICK=${INPUT}/DataMC_quick
OUTPUT_FAKE=${INPUT}/DataMC_fake

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi
  if [[ ! -d ${OUTPUT_QUICK} ]] ; then
    mkdir -p ${OUTPUT_QUICK}
  fi  
  if [[ ! -d ${OUTPUT_FAKE} ]] ; then
    mkdir -p ${OUTPUT_FAKE}
  fi
  
  makePlots --doSummary --quick --json ${JSON_PATH}/plot${YEAR}-inj-quick.json  --outDir ${OUTPUT_QUICK} --inDir ${INPUT} --variables variables2017.json --cuts variables2017.json
  makePlots --doSummary         --json ${JSON_PATH}/plot${YEAR}-inj.json        --outDir ${OUTPUT}       --inDir ${INPUT} --variables variables2017.json --cuts variables2017.json
  makePlots --doSummary --quick --json ${JSON_PATH}/plot${YEAR}-fake.json       --outDir ${OUTPUT}       --inDir ${INPUT} --variables variables2017-fakeRateMethod.json --cuts variables2017-fakeRateMethod.json --dofakeratio
fi
