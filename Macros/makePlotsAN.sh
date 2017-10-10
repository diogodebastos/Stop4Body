#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

INPUT=~/local-area/Stop4Body/nTuples_v2017-09-20
INPUT_TEST=~/local-area/Stop4Body/nTuples_v2017-09-20_test
INPUT_SWAP=~/local-area/Stop4Body/nTuples_v2017-08-13_swap
OUTPUT=~/local-area/Stop4Body/ANPlots/
#OUTPUT=./test/

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}/DataMC/AR
    mkdir -p ${OUTPUT}/DataMC/LNT
    mkdir -p ${OUTPUT}/DataMC/VR1
    mkdir -p ${OUTPUT}/DataMC/VR2
    mkdir -p ${OUTPUT}/DataMC/VR3
    mkdir -p ${OUTPUT}/Corr/AR
    mkdir -p ${OUTPUT}/Corr/LNT
    mkdir -p ${OUTPUT}/Corr/VR1
    mkdir -p ${OUTPUT}/Corr/VR2
    mkdir -p ${OUTPUT}/Corr/VR3
  fi

  VARIABLE_JSON=variablesAN_DataMC.json

  JSONFILE=variablesAN_DataMC.json
  makePlots --json ${JSON_PATH}/plot2016_lep.json --outDir ${OUTPUT}/DataMC/AR/ --inDir ${INPUT_TEST}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE}
  makeTwoDPlots --json ${JSON_PATH}/plot2016_lep.json --outDir ${OUTPUT}/Corr/AR/ --inDir ${INPUT_TEST}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE}

  JSONFILE=variablesAN_DataMC_LNT.json
  makePlots --json ${JSON_PATH}/plot2016_lep.json --outDir ${OUTPUT}/DataMC/LNT/ --inDir ${INPUT}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE}
  makeTwoDPlots --json ${JSON_PATH}/plot2016_lep.json --outDir ${OUTPUT}/Corr/LNT/ --inDir ${INPUT}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE}

  JSONFILE=variablesAN_DataMC_VR1.json
  makePlots --json ${JSON_PATH}/plot2016swap_lep.json --outDir ${OUTPUT}/DataMC/VR1/ --inDir ${INPUT_SWAP}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE}
  makeTwoDPlots --json ${JSON_PATH}/plot2016swap_lep.json --outDir ${OUTPUT}/Corr/VR1/ --inDir ${INPUT_SWAP}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE}

  JSONFILE=variablesAN_DataMC_VR2.json
  makePlots --json ${JSON_PATH}/plot2016_lep.json --outDir ${OUTPUT}/DataMC/VR2/ --inDir ${INPUT}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE}
  makeTwoDPlots --json ${JSON_PATH}/plot2016_lep.json --outDir ${OUTPUT}/Corr/VR2/ --inDir ${INPUT}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE}

  JSONFILE=variablesAN_DataMC_VR3.json
  makePlots --json ${JSON_PATH}/plot2016_lep.json --outDir ${OUTPUT}/DataMC/VR3/ --inDir ${INPUT}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE}
  makeTwoDPlots --json ${JSON_PATH}/plot2016_lep.json --outDir ${OUTPUT}/Corr/VR3/ --inDir ${INPUT}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE}

  for deltaM in 10 20 30 40 50 60 70 80; do
    mkdir -p ${OUTPUT}/BDT/DeltaM${deltaM}/AR
    mkdir -p ${OUTPUT}/BDT/DeltaM${deltaM}/LNT
    mkdir -p ${OUTPUT}/BDT/DeltaM${deltaM}/VR1
    mkdir -p ${OUTPUT}/BDT/DeltaM${deltaM}/VR2
    mkdir -p ${OUTPUT}/BDT/DeltaM${deltaM}/VR3

    VARIABLE_JSON=variablesAN_BDT_AR.json

    JSONFILE=variablesAN_BDT_AR.json
    makePlots --json ${JSON_PATH}/plot2016_DM${deltaM}RP.json --outDir ${OUTPUT}/BDT/DeltaM${deltaM}/AR --inDir ${INPUT_TEST}_bdt${deltaM}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE} --suffix bdt

    JSONFILE=variablesAN_BDT_LNT.json
    makePlots --json ${JSON_PATH}/plot2016_DM${deltaM}RP.json --outDir ${OUTPUT}/BDT/DeltaM${deltaM}/LNT --inDir ${INPUT}_bdt${deltaM}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE} --suffix bdt --unblind

    JSONFILE=variablesAN_BDT_VR1.json
    makePlots --json ${JSON_PATH}/plot2016swap_DM${deltaM}RP.json --outDir ${OUTPUT}/BDT/DeltaM${deltaM}/VR1 --inDir ${INPUT_SWAP}_bdt${deltaM}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE} --suffix bdt --unblind

    JSONFILE=variablesAN_BDT_VR2.json
    makePlots --json ${JSON_PATH}/plot2016_DM${deltaM}RP.json --outDir ${OUTPUT}/BDT/DeltaM${deltaM}/VR2 --inDir ${INPUT}_bdt${deltaM}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE} --suffix bdt --unblind

    JSONFILE=variablesAN_BDT_VR3.json
    makePlots --json ${JSON_PATH}/plot2016_DM${deltaM}RP.json --outDir ${OUTPUT}/BDT/DeltaM${deltaM}/VR3 --inDir ${INPUT}_bdt${deltaM}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE} --suffix bdt --unblind
  done
fi
