#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

INPUT=~/local-area/Stop4Body/nTuples_v2017-08-13
INPUT_TEST=~/local-area/Stop4Body/nTuples_v2017-08-13_test
INPUT_SWAP=~/local-area/Stop4Body/nTuples_v2017-08-13_swap
OUTPUT=~/local-area/Stop4Body/ANPlots/
#OUTPUT=./test/

if [[ -d ${INPUT} ]] ; then
  for deltaM in 10 20 30 40 50 60 70 80; do
    mkdir -p ${OUTPUT}/BDT/DeltaM${deltaM}/AR_unblinded

    VARIABLE_JSON=variablesAN_BDT_AR.json

    JSONFILE=variablesAN_BDT_AR.json
    makePlots --json ${JSON_PATH}/plot2016_DM${deltaM}RP.json --outDir ${OUTPUT}/BDT/DeltaM${deltaM}/AR_unblinded --inDir ${INPUT_TEST}_bdt${deltaM}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE} --suffix bdt --unblind
  done
fi
