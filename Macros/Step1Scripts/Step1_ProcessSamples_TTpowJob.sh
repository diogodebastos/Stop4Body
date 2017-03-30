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
JSONFILE=$JSON_PATH/TT_pow.json
MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --json $JSONFILE --outDir ${NTUPLE_DIR}"
eval $MY_PROCESS_SAMPLES_CMD

splitTrainingTest --json $JSONFILE --inDir ${NTUPLE_DIR} --testOutDir ${TEST_DIR} --trainOutDir ${TRAIN_DIR}
