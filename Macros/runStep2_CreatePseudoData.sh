#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

if [[ -d ${NTUPLE_DIR} ]]; then
  makePseudoData --json ${JSON_PATH}/makePD_Inj_300_270.json --inDir ${NTUPLE_DIR} --outDir ${NTUPLE_DIR} --noPresel --injectSignal
  mv ${NTUPLE_DIR}/PseudoData.root ${NTUPLE_DIR}/PseudoData_Injected_300_270.root

  makePseudoData --json ${JSON_PATH}/makePD.json             --inDir ${NTUPLE_DIR} --outDir ${NTUPLE_DIR} --noPresel
fi

