#!/bin/bash

OUTPUT=~/local-area/Stop4Body/BDT_Fits_Fix/
OUTPUT_INJ=~/local-area/Stop4Body/BDT_Fits_Injected_Fix/

if [[ ! -d ${OUTPUT} ]] ; then
  mkdir -p ${OUTPUT}
fi

if [[ ! -d ${OUTPUT_INJ} ]] ; then
  mkdir -p ${OUTPUT_INJ}
fi

BDTStudy --json samplesInj2Run.json --outDir ${OUTPUT_INJ}
BDTStudy --json samples2Run.json --outDir ${OUTPUT}
