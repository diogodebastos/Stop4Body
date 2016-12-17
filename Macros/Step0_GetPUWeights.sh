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
  getPUWeight --json ${JSON_PATH}/allMC.json  --outDir ${NTUPLE_DIR} --dataPU DataPileupHistogram.root


  #makePseudoData --lumi 10000 --json ${JSON_PATH}/makePD_Inj_300_270.json --inDir ${NTUPLE_DIR} --outDir ${NTUPLE_DIR} --noPresel --injectSignal
  #mv ${NTUPLE_DIR}/PseudoData.root ${NTUPLE_DIR}/PseudoData_Injected_300_270.root

  #makePseudoData --lumi 10000 --json ${JSON_PATH}/makePD.json             --inDir ${NTUPLE_DIR} --outDir ${NTUPLE_DIR} --noPresel

  #ln -s ${NTUPLE_DIR}/PseudoData*.root ${TEST_DIR}/
  #ln -s ${NTUPLE_DIR}/PseudoData*.root ${TRAIN_DIR}/
fi

