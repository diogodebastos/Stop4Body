#!/bin/bash

INPUT=~cbeiraod/local-area/Stop4Body/nTuples_v2016-08-31_bdt/
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

BDTStudy --json samplesInj2Run.json --outDir ${OUTPUT_INJ} --inDir ${INPUT} --suffix bdt
BDTStudy --json samples2Run.json --outDir ${OUTPUT} --inDir ${INPUT} --suffix bdt
BDTStudy --json samples2RunData.json --outDir ${OUTPUT_DAT} --inDir ${INPUT} --suffix bdt --lumi 1700
