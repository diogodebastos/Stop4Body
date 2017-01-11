#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

#INPUT=${NTUPLE_DIR}
#INPUT=${TRAIN_DIR}
INPUT=${TEST_DIR}
OUTPUT=${INPUT}_bdt

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi

  applyMVA --json ${JSON_PATH}/allSamples.json --method BDT --inDir ${INPUT}/ --outDir ${OUTPUT}/
fi

