#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

INPUT=${TEST_DIR}
OUTPUT=${BDT_DIR}

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi

  applyMVA --json ${JSON_PATH}/allSamples.json --method BDT --inDir ${INPUT}/ --outDir ${OUTPUT}/
fi

