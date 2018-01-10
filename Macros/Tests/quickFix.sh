#!/bin/bash

cd ..
. setupJSONs.sh
. setupPaths.sh
cd -


INPUT=~/local-area/Stop4Body/nTuples_v2017-10-19_test
OUTPUT=./plots/

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi

  VARIABLE_JSON=quickFix.json

  JSONFILE=quickFix.json

  for deltaM in 10 20 30 40 50 60 70 80; do
    mkdir -p ${OUTPUT}/DeltaM${deltaM}/

    makePlots --json ${JSON_PATH}/plot2016_DM${deltaM}RP.json --outDir ${OUTPUT}/DeltaM${deltaM}/ --inDir ${INPUT}_bdt${deltaM} --variables ${VARIABLE_JSON} --cuts ${JSONFILE} --suffix bdt --unblind
  done

else
  echo "The input directory does not exist"
fi

