#!/bin/bash

. ../setupJSONs.sh
. ../setupPaths.sh

PLOT_DIR=./scanPlots
SAMPLE_JSON=${JSON_PATH}/plot2016BCD.json

if [[ ! -d ${PLOT_DIR} ]] ; then
  mkdir -p ${PLOT_DIR}
fi

makePlots --json ${SAMPLE_JSON} --outDir ${PLOT_DIR} --inDir ~/local-area/Stop4Body/nTuples_v2017-02-09/ --variables scan.json --cuts scan.json
