#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

#OUTPUT_PD=~cbeiraod/local-area/Stop4Body/PseudoDataMC/
#OUTPUT_Data=~cbeiraod/local-area/Stop4Body/DataMC/

OUTPUT=./plots/
OUTPUT_LO=./plots_lo/

if [[ -d ${TEST_DIR} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi
  if [[ ! -d ${OUTPUT_LO} ]] ; then
    mkdir -p ${OUTPUT_LO}
  fi

  getYields --json ${JSON_PATH}/plot2016B.json    --inDir ${TEST_DIR} --variables variables.json --outDir ${OUTPUT}    --lumi 5877
  getYields --json ${JSON_PATH}/plot2016B_LO.json --inDir ${TEST_DIR} --variables variables.json --outDir ${OUTPUT_LO} --lumi 5877
  #getYields --json samples2Run.json --inDir ${INPUT} --variables variables.json --outDir ${OUTPUT_PD}
  #getYields --json samples2RunData.json --inDir ${INPUT} --variables variables.json --outDir ${OUTPUT_Data} --lumi 1700
fi

