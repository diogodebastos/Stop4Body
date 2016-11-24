#!/bin/bash

SIGNAL=~cbeiraod/local-area/Stop4Body/nTuples_v2016-11-23_train/T2DegStop_300_270.root
BACKGROUND=~cbeiraod/local-area/Stop4Body/nTuples_v2016-11-23_train/Background_TTbar.root

# If test files are not defined, the trainMVA binary will assume the supplied file is to be split into train and test by TMVA
SIGNAL_TEST=~cbeiraod/local-area/Stop4Body/nTuples_v2016-11-23_test/T2DegStop_300_270.root
BACKGROUND_TEST=~cbeiraod/local-area/Stop4Body/nTuples_v2016-11-23_test/Background_TTbar.root

if [[ -f ${SIGNAL} && -f ${BACKGROUND} ]] ; then
  trainMVA --method BDT --signalFile ${SIGNAL} --backgroundFile ${BACKGROUND} --testSignalFile ${SIGNAL_TEST} --testBackgroundFile ${BACKGROUND_TEST}
  root runTMVAGui.C
fi

