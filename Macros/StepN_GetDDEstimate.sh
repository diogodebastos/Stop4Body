#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

INPUT=${TEST_DIR}_bdt
OUTPUT=~cbeiraod/local-area/Stop4Body/DDEstimate
OUTPUT_POW=~cbeiraod/local-area/Stop4Body/DDEstimate_pow
OUTPUT_LEP=~cbeiraod/local-area/Stop4Body/DDEstimate_lep
INPUT_SWAP=${SWAP_DIR}_bdt
OUTPUT_SWAP=~cbeiraod/local-area/Stop4Body/DDEstimate_swap
OUTPUT_SWAP_POW=~cbeiraod/local-area/Stop4Body/DDEstimate_swap_pow
OUTPUT_SWAP_LEP=~cbeiraod/local-area/Stop4Body/DDEstimate_swap_lep


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

  getDDEstimate --json ${JSON_PATH}/plot2016.json     --outDir ${OUTPUT}     --inDir ${INPUT} --suffix bdt
  getDDEstimate --json ${JSON_PATH}/plot2016_pow.json --outDir ${OUTPUT_POW} --inDir ${INPUT} --suffix bdt
  getDDEstimate --json ${JSON_PATH}/plot2016_lep.json --outDir ${OUTPUT_LEP} --inDir ${INPUT} --suffix bdt
fi

if [[ -d ${INPUT_SWAP} ]] ; then
  if [[ ! -d ${OUTPUT_SWAP} ]] ; then
    mkdir -p ${OUTPUT_SWAP}
  fi
  if [[ ! -d ${OUTPUT_SWAP_POW} ]] ; then
    mkdir -p ${OUTPUT_SWAP_POW}
  fi
  if [[ ! -d ${OUTPUT_SWAP_LEP} ]] ; then
    mkdir -p ${OUTPUT_SWAP_LEP}
  fi

  getDDEstimate --json ${JSON_PATH}/plot2016swap.json --outDir ${OUTPUT_SWAP} --inDir ${INPUT_SWAP} --suffix bdt
  getDDEstimate --json ${JSON_PATH}/plot2016swap_pow.json --outDir ${OUTPUT_SWAP_POW} --inDir ${INPUT_SWAP} --suffix bdt
  getDDEstimate --json ${JSON_PATH}/plot2016swap_lep.json --outDir ${OUTPUT_SWAP_LEP} --inDir ${INPUT_SWAP} --suffix bdt

  makePlots --json ${JSON_PATH}/plot2016swap.json --outDir ${OUTPUT_SWAP} --inDir ${INPUT_SWAP} --variables variablesClosure.json --cuts variablesClosure.json
fi
