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
MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --json $JSON_PATH/allMC.json --outDir ${NTUPLE_DIR}"
eval $MY_PROCESS_SAMPLES_CMD

splitTrainingTest --json $JSON_PATH/allMC.json --inDir ${NTUPLE_DIR} --testOutDir ${TEST_DIR} --trainOutDir ${TRAIN_DIR}

hadd ${NTUPLE_DIR}/Background_TTbar.root ${NTUPLE_DIR}/Wjets_100to200.root ${NTUPLE_DIR}/Wjets_200to400.root ${NTUPLE_DIR}/Wjets_400to600.root ${NTUPLE_DIR}/Wjets_600to800.root ${NTUPLE_DIR}/Wjets_800to1200.root ${NTUPLE_DIR}/Wjets_1200to2500.root ${NTUPLE_DIR}/Wjets_2500toInf.root ${NTUPLE_DIR}/TTJets.root
hadd ${NTUPLE_DIR}/Background_TTbarLO.root ${NTUPLE_DIR}/Wjets_100to200.root ${NTUPLE_DIR}/Wjets_200to400.root ${NTUPLE_DIR}/Wjets_400to600.root ${NTUPLE_DIR}/Wjets_600to800.root ${NTUPLE_DIR}/Wjets_800to1200.root ${NTUPLE_DIR}/Wjets_1200to2500.root ${NTUPLE_DIR}/Wjets_2500toInf.root ${NTUPLE_DIR}/TTJets_LO.root
hadd ${TEST_DIR}/Background_TTbar.root ${TEST_DIR}/Wjets_100to200.root ${TEST_DIR}/Wjets_200to400.root ${TEST_DIR}/Wjets_400to600.root ${TEST_DIR}/Wjets_600to800.root ${TEST_DIR}/Wjets_800to1200.root ${TEST_DIR}/Wjets_1200to2500.root ${TEST_DIR}/Wjets_2500toInf.root ${TEST_DIR}/TTJets.root
hadd ${TEST_DIR}/Background_TTbarLO.root ${TEST_DIR}/Wjets_100to200.root ${TEST_DIR}/Wjets_200to400.root ${TEST_DIR}/Wjets_400to600.root ${TEST_DIR}/Wjets_600to800.root ${TEST_DIR}/Wjets_800to1200.root ${TEST_DIR}/Wjets_1200to2500.root ${TEST_DIR}/Wjets_2500toInf.root ${TEST_DIR}/TTJets_LO.root
hadd ${TRAIN_DIR}/Background_TTbar.root ${TRAIN_DIR}/Wjets_100to200.root ${TRAIN_DIR}/Wjets_200to400.root ${TRAIN_DIR}/Wjets_400to600.root ${TRAIN_DIR}/Wjets_600to800.root ${TRAIN_DIR}/Wjets_800to1200.root ${TRAIN_DIR}/Wjets_1200to2500.root ${TRAIN_DIR}/Wjets_2500toInf.root ${TRAIN_DIR}/TTJets.root
hadd ${TRAIN_DIR}/Background_TTbarLO.root ${TRAIN_DIR}/Wjets_100to200.root ${TRAIN_DIR}/Wjets_200to400.root ${TRAIN_DIR}/Wjets_400to600.root ${TRAIN_DIR}/Wjets_600to800.root ${TRAIN_DIR}/Wjets_800to1200.root ${TRAIN_DIR}/Wjets_1200to2500.root ${TRAIN_DIR}/Wjets_2500toInf.root ${TRAIN_DIR}/TTJets_LO.root
