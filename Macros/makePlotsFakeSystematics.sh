#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

INPUT=${NTUPLE_DIR}
OUTPUT=~/local-area/Stop4Body/FakeSystematics
#OUTPUT=./test/

if [[ -d ${INPUT} ]] ; then
  for deltaM in 10 20 30 40 50 60 70 80; do
    mkdir -p ${OUTPUT}/DeltaM${deltaM}/
    mkdir -p ${OUTPUT}_NU/DeltaM${deltaM}/

    JSONFILE=fake_bdt${deltaM}.json
    fakeSystematics --json ${JSON_PATH}/plot2016_lep.json --outDir ${OUTPUT}/DeltaM${deltaM} --inDir ${INPUT}_bdt${deltaM}/ --suffix bdt --variables fake_bdt10.json --cuts ${JSONFILE}
    fakeSystematics --json ${JSON_PATH}/plot2016_lep.json --outDir ${OUTPUT}_NU/DeltaM${deltaM} --inDir ${INPUT}_bdt${deltaM}/ --suffix bdt --variables fake_bdt10.json --cuts ${JSONFILE} --doNonUniversality
  done
fi
