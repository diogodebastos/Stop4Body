#!/bin/bash

. ../setupJSONs.sh
. ../setupPaths.sh

PLOT_DIR=./scanPlots
SAMPLE_JSON=${JSON_PATH}/plot2016BCD.json

if [[ ! -d ${PLOT_DIR} ]] ; then
  mkdir -p ${PLOT_DIR}
fi

makePlots --json ${SAMPLE_JSON} --outDir ${PLOT_DIR} --inDir ${NTUPLE_DIR} --variables scan.json --cuts scan.json
