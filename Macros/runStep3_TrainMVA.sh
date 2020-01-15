#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

#SIGNAL=T2DegStop_500_420.root
SIGNAL=T2DegStop_deltaM10.root
BACKGROUND=Background_TTbar_WJets.root

SIGNAL_TRAIN=${TRAIN_DIR}/${SIGNAL}
BACKGROUND_TRAIN=${TRAIN_DIR}/${BACKGROUND}

# If test files are not defined, the trainMVA binary will assume the supplied file is to be split into train and test by TMVA
SIGNAL_TEST=${TEST_DIR}/${SIGNAL}
BACKGROUND_TEST=${TEST_DIR}/${BACKGROUND}

#trainMVA --method BDT --signalFile ${SIGNAL} --backgroundFile ${BACKGROUND} --testSignalFile ${SIGNAL_TEST} --testBackgroundFile ${BACKGROUND_TEST}



if [[ -f ${SIGNAL_TRAIN} && -f ${BACKGROUND_TRAIN} ]] ; then
  echo "Training BDT..."
  # Use isHighDeltaM for dm=70;80
  trainMVA --method BDT --signalFile ${SIGNAL_TRAIN} --backgroundFile ${BACKGROUND_TRAIN} --testSignalFile ${SIGNAL_TEST} --testBackgroundFile ${BACKGROUND_TEST} #--isHighDeltaM
  root runTMVAGui.C
fi
