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

if [[ -d ${NTUPLE_DIR} ]]; then
  makePseudoData --lumi 30000 --noPresel                --json ${JSON_PATH}/makePD.json             --inDir ${NTUPLE_DIR} --outFile ${NTUPLE_DIR}/PseudoData.root

  ln -s ${NTUPLE_DIR}/PseudoData.root ${TEST_DIR}/
  ln -s ${NTUPLE_DIR}/PseudoData.root ${TRAIN_DIR}/
fi

