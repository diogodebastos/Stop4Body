#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

INPUT1=${TEST_DIR}
INPUT2=${TRAIN_DIR}
OUTPUT1=${BDTTEST_DIR}
OUTPUT2=${BDTTRAIN_DIR}

if [[ -d ${INPUT1} ]] ; then
  if [[ ! -d ${OUTPUT1} ]] ; then
    mkdir -p ${OUTPUT1}
  fi

  applyMVA --json ${JSON_PATH}/allSamples.json --method BDT --inDir ${INPUT1}/ --outDir ${OUTPUT1}/
fi

if [[ -d ${INPUT2} ]] ; then
  if [[ ! -d ${OUTPUT2} ]] ; then
    mkdir -p ${OUTPUT2}
  fi

  applyMVA --json ${JSON_PATH}/allSamples.json --method BDT --inDir ${INPUT2}/ --outDir ${OUTPUT2}/
fi

