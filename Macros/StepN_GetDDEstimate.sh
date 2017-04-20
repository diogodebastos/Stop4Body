#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

INPUT=${TEST_DIR}_bdt
OUTPUT="~cbeiraod/local-area/Stop4Body/DDEstimate"
OUTPUT_POW="~cbeiraod/local-area/Stop4Body/DDEstimate_pow"
OUTPUT_LEP="~cbeiraod/local-area/Stop4Body/DDEstimate_lep"


if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi
  if [[ ! -d ${OUTPUT_POW} ]] ; then
    mkdir -p ${OUTPUT_POW}
  fi
  if [[ ! -d ${OUTPUT_LEP} ]] ; then
    mkdir -p ${OUTPUT_LEP}
  fi

  getDDEstimate --json JSON/plot2016.json     --outDir ${OUTPUT}     --inDir ${INPUT} --suffix bdt
  getDDEstimate --json JSON/plot2016_pow.json --outDir ${OUTPUT_POW} --inDir ${INPUT} --suffix bdt
  getDDEstimate --json JSON/plot2016_lep.json --outDir ${OUTPUT_LEP} --inDir ${INPUT} --suffix bdt
fi
