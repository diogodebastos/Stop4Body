#!/bin/bash

cd ..
. setupJSONs.sh
. setupPaths.sh
cd -


INPUT=~/local-area/Stop4Body/nTuples_v2017-08-13
OUTPUT=./plots/

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi

  VARIABLE_JSON=variablesNcuts.json

  JSONFILE=variablesNcuts.json

  makePlots --json ${JSON_PATH}/plot2016_DM10RP.json --outDir ${OUTPUT} --inDir ${INPUT} --variables ${VARIABLE_JSON} --cuts ${JSONFILE} --suffix bdt --unblind
else
  echo "The input directory does not exist"
fi

