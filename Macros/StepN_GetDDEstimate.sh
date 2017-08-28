#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

BDT=bdt80
INPUT=${BDT_DIR}
INPUT=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-06-05_SysVar_test_${BDT}
OUTPUT=~cbeiraod/local-area/Stop4Body/DDEstimate_${BDT}
OUTPUT_POW=~cbeiraod/local-area/Stop4Body/DDEstimate_${BDT}_pow
OUTPUT_LEP=~cbeiraod/local-area/Stop4Body/DDEstimate_${BDT}_lep
LNTDIR=${LNTBDT_DIR}

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

  #getDDEstimate --json ${JSON_PATH}/plot2016.json     --outDir ${OUTPUT}     --inDir ${INPUT} --suffix bdt --signalRegionCut 0.31
  #getDDEstimate --json ${JSON_PATH}/plot2016_pow.json --outDir ${OUTPUT_POW} --inDir ${INPUT} --suffix bdt --signalRegionCut 0.31
  getDDEstimate --json ${JSON_PATH}/plot2016_lep.json --outDir ${OUTPUT_LEP} --inDir ${INPUT} --suffix bdt --signalRegionCut 0.44 --isHighDeltaM

  #makePlots --json ${JSON_PATH}/plot2016.json --outDir ${OUTPUT} --inDir ${INPUT} --variables variablesSR.json --cuts variablesSR.json --suffix bdt
  #makePlots --json ${JSON_PATH}/plot2016_pow.json --outDir ${OUTPUT_POW} --inDir ${INPUT} --variables variablesSR.json --cuts variablesSR.json --suffix bdt
  makePlots --json ${JSON_PATH}/plot2016_lep.json --outDir ${OUTPUT_LEP} --inDir ${INPUT} --variables variablesSR.json --cuts variablesSR.json --suffix bdt
fi


INPUT_SWAP=${SWAPBDT_DIR}
OUTPUT_SWAP=~cbeiraod/local-area/Stop4Body/DDEstimate_swap
OUTPUT_SWAP_POW=~cbeiraod/local-area/Stop4Body/DDEstimate_swap_pow
OUTPUT_SWAP_LEP=~cbeiraod/local-area/Stop4Body/DDEstimate_swap_lep

#if [[ -d ${INPUT_SWAP} ]] ; then
  #if [[ ! -d ${OUTPUT_SWAP} ]] ; then
  #  mkdir -p ${OUTPUT_SWAP}
  #fi
  #if [[ ! -d ${OUTPUT_SWAP_POW} ]] ; then
  #  mkdir -p ${OUTPUT_SWAP_POW}
  #fi
  #if [[ ! -d ${OUTPUT_SWAP_LEP} ]] ; then
  #  mkdir -p ${OUTPUT_SWAP_LEP}
  #fi

  #getDDEstimate --json ${JSON_PATH}/plot2016swap.json --outDir ${OUTPUT_SWAP} --inDir ${INPUT_SWAP} --suffix bdt
  #getDDEstimate --json ${JSON_PATH}/plot2016swap_pow.json --outDir ${OUTPUT_SWAP_POW} --inDir ${INPUT_SWAP} --suffix bdt
  #getDDEstimate --json ${JSON_PATH}/plot2016swap_lep.json --outDir ${OUTPUT_SWAP_LEP} --inDir ${INPUT_SWAP} --suffix bdt

  #makePlots --json ${JSON_PATH}/plot2016swap.json --outDir ${OUTPUT_SWAP} --inDir ${INPUT_SWAP} --variables variablesClosure.json --cuts variablesClosure.json --suffix bdt --unblind
  #makePlots --json ${JSON_PATH}/plot2016swap_pow.json --outDir ${OUTPUT_SWAP_POW} --inDir ${INPUT_SWAP} --variables variablesClosure.json --cuts variablesClosure.json --suffix bdt --unblind
  #makePlots --json ${JSON_PATH}/plot2016swap_lep.json --outDir ${OUTPUT_SWAP_LEP} --inDir ${INPUT_SWAP} --variables variablesClosure.json --cuts variablesClosure.json --suffix bdt --unblind
#fi
