#!/bin/bash

alias cmsenv='eval `scramv1 runtime -sh`'

cd /exper-sw/cmst3/cmssw/users/cbeiraod/
. setup.sh

cd $CMSSW_BASE/src/
eval `scramv1 runtime -sh`

cd UserCode/Stop4Body/Macros/

. setupPaths.sh

JSON_FILE=${JSON_PATH}/Orig/SignalPoints/Sig_300_270.json

getPUWeight --json ${JSON_FILE}  --outDir ${SYNC_DIR} --dataPU DataPileupHistogram.root --otherMCPU MCPileupHistogram.root

mv ${SYNC_DIR}/puWeights_Sig_300_270.json.root ${SYNC_DIR}/puWeights.root

processSamples --json ${JSON_FILE} --outDir ${SYNC_DIR} --doSync 300 --noSkim
processSamples --json ${JSON_FILE} --outDir ${SYNC_DIR}              --noSkim
