#!/bin/bash

source localUserSetup.sh

export YEAR=2018

export BASE_PATH=$CMSSW_BASE/src/UserCode/Stop4Body-nanoAOD
export MACROS_PATH=$BASE_PATH/Macros
export JSON_PATH=$MACROS_PATH/JSON/${YEAR}

export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples18_nanoAOD_v2020-03-20
export TRAIN_DIR=${NTUPLE_DIR}_train
export TEST_DIR=${NTUPLE_DIR}_test

# USED DIRS FOR 2017 AN
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples_v2019-06-05-EWK_IRS-low-Mu-SFs
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples_v2019-06-05-FastSim
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples_v2019-06-05-QCD-Mu
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples_v2019-05-16-updated
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples_v2019-05-06-skimmed
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples_v2019-02-07-noSkim
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples_v2019-04-02-fakeMCClosure
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples_v2019-04-30-skimmed
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples_v2019-05-10-WJets-closure
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples_v2019-05-10-TTbar-closure
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples_v2019-05-10-ZInv-QCDratio-closure

export SYNC_DIR=${BASE_NTUPLE_PATH}/nTuples_sync
export PU_TEST_DIR=${BASE_NTUPLE_PATH}/nTuples_PU_test

export CURRENT_BDT=bdt80

export TRAIN_DIR=${NTUPLE_DIR}_train
export TEST_DIR=${NTUPLE_DIR}_test

#For debug while splitting is not ready
#export TRAIN_DIR=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_train
#export TEST_DIR=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test

export NTUPLE_DIR_16=${BASE_NTUPLE_PATH}/nTuples16_v2017-10-19
export TRAIN_DIR_16=${NTUPLE_DIR_16}_train
export TEST_DIR_16=${NTUPLE_DIR_16}_test

export BDTTEST_DIR=${TEST_DIR}_${CURRENT_BDT}
export BDTTRAIN_DIR=${TRAIN_DIR}_${CURRENT_BDT}
export BDT_DIR=${TEST_DIR}_${CURRENT_BDT}

export SWAP_DIR=${NTUPLE_DIR}_swap
export SWAPBDT_DIR=${SWAP_DIR}_${CURRENT_BDT}
export LNT_DIR=${NTUPLE_DIR}_looseNotTight
export LNTBDT_DIR=${LNT_DIR}_${CURRENT_BDT}

export LHESCALE_DIR=${BASE_NTUPLE_PATH}/lheWeightScale/${YEAR}
export PUWEIGHT_DIR=${BASE_NTUPLE_PATH}/puWeights/${YEAR}

export LIMIT_DIR=${BASE_NTUPLE_PATH}/Limits/

#export PROCESS_SAMPLES_CMD="processSamples --jetPtThreshold 20"
export PROCESS_SAMPLES_CMD="processSamples --preemptiveDropEvents --doPromptTagging --doLooseLeptons --year ${YEAR}"
export PROCESS_SAMPLES_CMD_LNT="${PROCESS_SAMPLES_CMD} --looseNotTight"
export PROCESS_SAMPLES_CMD_LOOSE="processSamples --preemptiveDropEvents --doLooseLeptons"
