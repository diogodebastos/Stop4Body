#!/bin/bash

#if [ $# -eq 0 ]
#  then
#    echo "No arguments supplied"
#    exit 1
#fi

. setupJSONs.sh
. setupPaths.sh

#CURRENT_BDT=bdt_$1

for DIR in "${NTUPLE_DIR}" "${TEST_DIR}" ;do
  echo "Processing directory: ${DIR}"
  OUTPUT=${DIR}_${CURRENT_BDT}
#  OUTPUT=${DIR}_bdt${CURRENT_BDT}

  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi

  #python applyMVA.py -i ${DIR} -o ${OUTPUT} -w weights_bdt${CURRENT_BDT}
  python applyMVA.py -i ${DIR} -o ${OUTPUT} -w weights_${CURRENT_BDT}
done

echo "Run the following commands to check the progress of the output and resubmit failed jobs:"
echo "  python checkJobs.py --noMerge -o ${NTUPLE_DIR}_${CURRENT_BDT}"
echo "  python checkJobs.py --noMerge -o ${TEST_DIR}_${CURRENT_BDT}"
