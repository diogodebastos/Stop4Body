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
cd UserCode/Stop4Body-nanoAOD/Macros/

. setupPaths.sh

if [[ -d ${PUWEIGHT_DIR} ]]; then
 if [ ${YEAR} == 2016 ]; then
  getLHEweightScales --json ${JSON_PATH}/Data.json  --outDir ${LHESCALE_DIR}
 elif [ ${YEAR} == 2017 ] || [ ${YEAR} == 2018 ]; then
  getLHEweightScales --json ${JSON_PATH}/DataMetHT.json  --outDir ${LHESCALE_DIR}
#  getLHEweightScales --json ${JSON_PATH}/analysisData.json  --outDir ${LHESCALE_DIR}
#  getLHEweightScales --json ${JSON_PATH}/allData.json  --outDir ${LHESCALE_DIR}
 fi
fi
