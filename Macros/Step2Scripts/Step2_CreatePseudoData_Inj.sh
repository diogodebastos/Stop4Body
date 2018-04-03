#!/bin/bash

alias cmsenv='eval `scramv1 runtime -sh`'

export SCRAM_ARCH=slc6_amd64_gcc530
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
export CMS_PATH=$VO_CMS_SW_DIR
source /cvmfs/cms.cern.ch/crab3/crab.sh

#cd /exper-sw/cmst3/cmssw/users/cbeiraod/CMSSW_8_0_14/src/
cd $CMSSW_BASE/src/
eval `scramv1 runtime -sh`

#cd /exper-sw/cmst3/cmssw/users/cbeiraod/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/
cd UserCode/Stop4Body/Macros/

. setupPaths.sh

if [[ -d ${NTUPLE_DIR} ]]; then
  makePseudoData --lumi 30000 --noPresel --injectSignal --json ${JSON_PATH}/makePD_Inj_300_270.json --inDir ${NTUPLE_DIR} --outFile ${NTUPLE_DIR}/PseudoData_Injected_300_270.root

  ln -s ${NTUPLE_DIR}/PseudoData_Injected_300_270.root ${TEST_DIR}/
  ln -s ${NTUPLE_DIR}/PseudoData_Injected_300_270.root ${TRAIN_DIR}/
fi

