#!/bin/bash

alias cmsenv='eval `scramv1 runtime -sh`'

cd /exper-sw/cmst3/cmssw/users/cbeiraod/
. setup.sh

#cd /exper-sw/cmst3/cmssw/users/cbeiraod/CMSSW_8_0_14/src/
cd $CMSSW_BASE/src/
eval `scramv1 runtime -sh`

#cd /exper-sw/cmst3/cmssw/users/cbeiraod/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/
cd UserCode/Stop4Body/Macros/PUTests/

. ../setupPaths.sh

if [[ -d ${PU_TEST_DIR} ]]; then
  getPUWeight --json ${JSON_PATH}/Orig/Data.json  --outDir ${PU_TEST_DIR} --dataPU ../DataPileupHistogram.root
fi

