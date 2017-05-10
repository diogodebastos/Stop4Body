#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

SIGNAL=${TRAIN_DIR}/T2DegStop_300_270.root
BACKGROUND=${TRAIN_DIR}/Background_TTLep.root

# If test files are not defined, the trainMVA binary will assume the supplied file is to be split into train and test by TMVA
SIGNAL_TEST=${TEST_DIR}/T2DegStop_300_270.root
BACKGROUND_TEST=${TEST_DIR}/Background_TTLep.root

if [[ -f ${SIGNAL} && -f ${BACKGROUND} ]] ; then
  trainMVA --method BDT --signalFile ${SIGNAL} --backgroundFile ${BACKGROUND} --testSignalFile ${SIGNAL_TEST} --testBackgroundFile ${BACKGROUND_TEST}
  root runTMVAGui.C
fi

