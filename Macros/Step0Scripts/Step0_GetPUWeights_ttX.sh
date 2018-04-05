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

if [[ -d ${PUWEIGHT_DIR} ]]; then
  getPUWeight --json ${JSON_PATH}/ttx.json  --outDir ${PUWEIGHT_DIR} --dataPU DataPileupHistogram.root --otherMCPU MCPileupHistogram.root
fi

