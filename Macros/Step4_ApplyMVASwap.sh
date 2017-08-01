#!/bin/bash

if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
    exit 1
fi

. setupJSONs.sh
. setupPaths.sh

CURRENT_BDT=$1

INPUT=${SWAP_DIR}
OUTPUT=${INPUT}_bdt${CURRENT_BDT}

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi

  applyMVA --json ${JSON_PATH}/allSamplesSwap.json --method BDT --inDir ${INPUT}/ --outDir ${OUTPUT}/ --altDir weights_bdt${CURRENT_BDT}
fi

