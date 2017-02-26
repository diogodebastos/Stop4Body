#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

INPUT=${TEST_DIR}_bdt/
OUTPUT=~cbeiraod/local-area/Stop4Body/BDT_Fits/
OUTPUT_INJ=~cbeiraod/local-area/Stop4Body/BDT_Fits_Injected/
OUTPUT_DAT=~cbeiraod/local-area/Stop4Body/BDT_Fits_Data/

if [[ ! -d ${OUTPUT} ]] ; then
  mkdir -p ${OUTPUT}
fi

if [[ ! -d ${OUTPUT_INJ} ]] ; then
  mkdir -p ${OUTPUT_INJ}
fi

if [[ ! -d ${OUTPUT_DAT} ]] ; then
  mkdir -p ${OUTPUT_DAT}
fi

BDTStudy --json ${JSON_PATH}/plot2016_LO.json --outDir ${OUTPUT_DAT} --inDir ${INPUT} --suffix bdt
BDTStudy --json ${JSON_PATH}/plot2016_PDInj_LO.json --outDir ${OUTPUT_INJ} --inDir ${INPUT} --suffix bdt
BDTStudy --json ${JSON_PATH}/plot2016_PD_LO.json --outDir ${OUTPUT} --inDir ${INPUT} --suffix bdt
