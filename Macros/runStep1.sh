#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

if [[ ! -d ${NTUPLE_DIR} ]] ; then
  mkdir -p ${NTUPLE_DIR}
  if [[ ! -d ${PUWEIGHT_DIR} ]] ; then
    echo "Please run Step0 first"
    exit 1
  fi
  if [[ ! -f ${PUWEIGHT_DIR}/puWeights.root ]] ; then
    echo "Please run Step0 first"
    exit 1
  fi
  cp ${PUWEIGHT_DIR}/puWeights.root ${NTUPLE_DIR}
  if [[ ! -d ${LHESCALE_DIR} ]] ; then
    echo "Please run Step0-LHEWeight first"
    exit 1
  fi
  if [[ ! -f ${LHESCALE_DIR}/lheWeights.root ]] ; then
    echo "Please run Step0-LHEWheigt_part2 first"
    exit 1
  fi
  cp ${LHESCALE_DIR}/lheWeights.root ${NTUPLE_DIR}
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
