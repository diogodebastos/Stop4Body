#!/bin/bash

. ../setupJSONs.sh
. ../setupPaths.sh

BASE_PLOT_DIR="./plots"
PLOTS_NO_PU=${BASE_PLOT_DIR}"_noPU"
#PLOTS_ALT=${BASE_PLOT_DIR}"_altPU"

if [[ ! -d ${BASE_PLOT_DIR} ]] ; then
  mkdir -p ${BASE_PLOT_DIR}
fi
if [[ ! -d ${PLOTS_NO_PU} ]] ; then
  mkdir -p ${PLOTS_NO_PU}
fi
#if [[ ! -d ${PLOTS_ALT} ]] ; then
#  mkdir -p ${PLOTS_ALT}
#fi

makePlots --json ${JSON_PATH}/puTests.json --inDir ${PU_TEST_DIR} --variables variables.json --outDir ${BASE_PLOT_DIR}
makePlots --json ${JSON_PATH}/puTests.json --inDir ${PU_TEST_DIR} --variables variables.json --outDir ${PLOTS_NO_PU}   --noPUweight
#makePlots --json ${JSON_PATH}/puTests.json --inDir ${PU_TEST_DIR} --variables variables.json --outDir ${PLOTS_ALT}     --
