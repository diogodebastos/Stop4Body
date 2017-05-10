#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

INPUT=${SWAP_DIR}
OUTPUT=${SWAPBDT_DIR}

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi

  applyMVA --json ${JSON_PATH}/allSamplesSwap.json --method BDT --inDir ${INPUT}/ --outDir ${OUTPUT}/
fi

