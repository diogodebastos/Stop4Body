#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

#OUTPUT_PD=~cbeiraod/local-area/Stop4Body/PseudoDataMC/
#OUTPUT_Data=~cbeiraod/local-area/Stop4Body/DataMC/

OUTPUT=./plots/
OUTPUT_POW=./plots_pow/
OUTPUT_LEP=./plots_lep/

if [[ -d ${TEST_DIR} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi
  if [[ ! -d ${OUTPUT_POW} ]] ; then
    mkdir -p ${OUTPUT_POW}
  fi
  if [[ ! -d ${OUTPUT_LEP} ]] ; then
    mkdir -p ${OUTPUT_LEP}
  fi

  getYields --json ${JSON_PATH}/plot2016B.json     --inDir ${NTUPLE_DIR} --variables variables.json --outDir ${OUTPUT}     --lumi 35866
  getYields --json ${JSON_PATH}/plot2016B_pow.json --inDir ${NTUPLE_DIR} --variables variables.json --outDir ${OUTPUT_POW} --lumi 35866
  getYields --json ${JSON_PATH}/plot2016B_lep.json --inDir ${NTUPLE_DIR} --variables variables.json --outDir ${OUTPUT_LEP} --lumi 35866
fi

