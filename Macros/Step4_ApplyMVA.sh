#!/bin/bash

INPUT=~cbeiraod/local-area/Stop4Body/nTuples_v2016-08-31/
OUTPUT=~cbeiraod/local-area/Stop4Body/nTuples_v2016-08-31_bdt/

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi

  applyMVA --json samples.json --method BDT --inDir ${INPUT} --outDir ${OUTPUT}
fi

