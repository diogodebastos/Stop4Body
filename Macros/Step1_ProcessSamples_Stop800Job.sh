#!/bin/bash

alias cmsenv='eval `scramv1 runtime -sh`'

cd /exper-sw/cmst3/cmssw/users/cbeiraod/
. setup.sh

#cd /exper-sw/cmst3/cmssw/users/cbeiraod/CMSSW_8_0_14/src/
cd $CMSSW_BASE/src/
eval `scramv1 runtime -sh`

#cd /exper-sw/cmst3/cmssw/users/cbeiraod/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/
cd UserCode/Stop4Body/Macros/

. setupPaths.sh
MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --json $JSON_PATH/stop800.json --outDir ${NTUPLE_DIR}"
eval $MY_PROCESS_SAMPLES_CMD

splitTrainingTest --json $JSON_PATH/stop800.json --inDir ${NTUPLE_DIR} --testOutDir ${TEST_DIR} --trainOutDir ${TRAIN_DIR}
