#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

if [[ ! -d ${NTUPLE_DIR} ]] ; then
  mkdir -p ${NTUPLE_DIR}
fi
if [[ ! -d ${TEST_DIR} ]] ; then
  mkdir -p ${TEST_DIR}
fi
if [[ ! -d ${TRAIN_DIR} ]] ; then
  mkdir -p ${TRAIN_DIR}
fi

python buildAndSubmitJobs.py -o ${NTUPLE_DIR}

echo "Run the following command to check the progress of the output and resubmit failed jobs:"
echo "  python checkJobs.py -o ${NTUPLE_DIR}"
