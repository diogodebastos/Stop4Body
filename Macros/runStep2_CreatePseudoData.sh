#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

if [[ -d ${NTUPLE_DIR} ]]; then
  makePseudoData --json ${JSON_PATH}/makePD_Inj_300_270.json --inDir ${NTUPLE_DIR} --outDir ${NTUPLE_DIR} --noPresel --injectSignal --suffix Injected_300_270

  makePseudoData --json ${JSON_PATH}/makePD.json             --inDir ${NTUPLE_DIR} --outDir ${NTUPLE_DIR} --noPresel
fi

