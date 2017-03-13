#!/bin/bash

. ../setupJSONs.sh
. ../setupPaths.sh

PLOT_DIR=./
SAMPLE_JSON=${JSON_PATH}/Orig/SignalPoints/Sig_300_270.json

makePlots --json ${SAMPLE_JSON} --outDir ${PLOT_DIR} --inDir ${SYNC_DIR} --variables cuts.json --cuts cuts.json --rawEvents
