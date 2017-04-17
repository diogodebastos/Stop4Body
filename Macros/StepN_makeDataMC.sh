#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

INPUT=${NTUPLE_DIR}
OUTPUT="./plots"


if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi

  makePlots --json ${JSON_PATH}/plot2016.json --outDir ${OUTPUT} --inDir ${INPUT} --variables variables.json --cuts variables.json
fi
