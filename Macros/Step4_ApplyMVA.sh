#!/bin/bash

if [ $# -eq 0 ]
  then
    echo "No arguments supplied"
    exit 1
fi

. setupJSONs.sh
. setupPaths.sh

CURRENT_BDT=$1

for DIR in "${NTUPLE_DIR}" "${TEST_DIR}" ;do
  echo "Processing directory: ${DIR}"
  OUTPUT=${DIR}_bdt${CURRENT_BDT}

  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi

  applyMVA --json ${JSON_PATH}/allSamples.json --method BDT --inDir ${DIR}/ --outDir ${OUTPUT}/ --altDir weights_bdt${CURRENT_BDT}
done

