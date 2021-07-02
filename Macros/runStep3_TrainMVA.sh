#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

#SIGNAL=T2DegStop_500_420.root
SIGNAL=T2DegStop_deltaM${CURRENT_DM}.root

if [ ${YEAR} == 2017 ]; then
  BACKGROUND=Background_TTbar_WJetsNLO.root
elif [ ${YEAR} == 2018 ]; then
  BACKGROUND=Background_TTbarPOW_WJetsNLO.root
fi
#BACKGROUND=Background_TTbar_WJets.root

SIGNAL_TRAIN=${TRAIN_DIR}/${SIGNAL}
BACKGROUND_TRAIN=${TRAIN_DIR}/${BACKGROUND}

# If test files are not defined, the trainMVA binary will assume the supplied file is to be split into train and test by TMVA
SIGNAL_TEST=${TEST_DIR}/${SIGNAL}
BACKGROUND_TEST=${TEST_DIR}/${BACKGROUND}

#trainMVA --method BDT --signalFile ${SIGNAL} --backgroundFile ${BACKGROUND} --testSignalFile ${SIGNAL_TEST} --testBackgroundFile ${BACKGROUND_TEST}

# Optimize variable cut
#WEIGHTS_DIR=weights${YEAR}_bdt${CURRENT_DM}_HT${HTCUT}_Jet1Pt${JET1PTCUT}
WEIGHTS_DIR=weights${YEAR}_bdt${CURRENT_DM}

if [[ ! -d ${WEIGHTS_DIR} ]] ; then
  mkdir -p ${WEIGHTS_DIR}
fi

if [[ -f ${SIGNAL_TRAIN} && -f ${BACKGROUND_TRAIN} ]] ; then
  echo "Training BDT for DM${CURRENT_DM}..."
  # Use isHighDeltaM for dm=70;80
  if [ ${CURRENT_DM} -gt "60" ] ; then
    # Optimize variable cut
    #trainMVA --method BDT --signalFile ${SIGNAL_TRAIN} --backgroundFile ${BACKGROUND_TRAIN} --testSignalFile ${SIGNAL_TEST} --testBackgroundFile ${BACKGROUND_TEST} --weightsDir ${WEIGHTS_DIR} --isHighDeltaM --htCut ${HTCUT} --jet1ptCut ${JET1PTCUT}
    trainMVA --method BDT --signalFile ${SIGNAL_TRAIN} --backgroundFile ${BACKGROUND_TRAIN} --testSignalFile ${SIGNAL_TEST} --testBackgroundFile ${BACKGROUND_TEST} --weightsDir ${WEIGHTS_DIR} --isHighDeltaM
  else
    # Optimize variable cut
    #trainMVA --method BDT --signalFile ${SIGNAL_TRAIN} --backgroundFile ${BACKGROUND_TRAIN} --testSignalFile ${SIGNAL_TEST} --testBackgroundFile ${BACKGROUND_TEST} --weightsDir ${WEIGHTS_DIR} --htCut ${HTCUT} --jet1ptCut ${JET1PTCUT}
    trainMVA --method BDT --signalFile ${SIGNAL_TRAIN} --backgroundFile ${BACKGROUND_TRAIN} --testSignalFile ${SIGNAL_TEST} --testBackgroundFile ${BACKGROUND_TEST} --weightsDir ${WEIGHTS_DIR}
  fi
  cd ${WEIGHTS_DIR}
  cp ../runTMVAGui.C .
  root -l runTMVAGui.C
fi
