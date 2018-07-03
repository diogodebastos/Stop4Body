#!/bin/bash

source localUserSetup.sh

export BASE_PATH=$CMSSW_BASE/src/UserCode/Stop4Body/
export JSON_PATH=$BASE_PATH/Macros/JSON/

export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples_v2018-07-02
export SYNC_DIR=${BASE_NTUPLE_PATH}/nTuples_sync
export PU_TEST_DIR=${BASE_NTUPLE_PATH}/nTuples_PU_test

export CURRENT_BDT=bdt10

export TRAIN_DIR=${NTUPLE_DIR}_train
export TEST_DIR=${NTUPLE_DIR}_test
export BDTTEST_DIR=${TEST_DIR}_${CURRENT_BDT}
export BDTTRAIN_DIR=${TRAIN_DIR}_${CURRENT_BDT}
export BDT_DIR=${TEST_DIR}_${CURRENT_BDT}

export SWAP_DIR=${NTUPLE_DIR}_swap
export SWAPBDT_DIR=${SWAP_DIR}_${CURRENT_BDT}
export LNT_DIR=${NTUPLE_DIR}_looseNotTight
export LNTBDT_DIR=${LNT_DIR}_${CURRENT_BDT}

export LHESCALE_DIR=${BASE_NTUPLE_PATH}/lheWeightScale
export PUWEIGHT_DIR=${BASE_NTUPLE_PATH}/puWeights

export LIMIT_DIR=${BASE_NTUPLE_PATH}/Limits/

#export PROCESS_SAMPLES_CMD="processSamples --jetPtThreshold 20"
export PROCESS_SAMPLES_CMD="processSamples --preemptiveDropEvents --doPromptTagging --doLooseLeptons"
export PROCESS_SAMPLES_CMD_LNT="${PROCESS_SAMPLES_CMD} --looseNotTight"
export PROCESS_SAMPLES_CMD_LOOSE="processSamples --preemptiveDropEvents --doLooseLeptons"
