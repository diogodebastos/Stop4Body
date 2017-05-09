#!/bin/bash

export BASE_PATH=$CMSSW_BASE/src/UserCode/Stop4Body/
export JSON_PATH=$BASE_PATH/Macros/JSON/

export NTUPLE_DIR=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-04-27
export SYNC_DIR=/lstore/cms/cbeiraod/Stop4Body/nTuples_sync
export PU_TEST_DIR=/lstore/cms/cbeiraod/Stop4Body/nTuples_PU_test

export TRAIN_DIR=${NTUPLE_DIR}_train
export TEST_DIR=${NTUPLE_DIR}_test
export BDT_DIR=${TEST_DIR}_bdt
export SWAP_DIR=${NTUPLE_DIR}_swap
export SWAPBDT_DIR=${SWAP_DIR}_bdt
export LNT_DIR=${NTUPLE_DIR}_looseNotTight
export LNTBDT_DIR=${LNT_DIR}_bdt


export PROCESS_SAMPLES_CMD="processSamples --jetPtThreshold 20"
export PROCESS_SAMPLES_CMD="processSamples --preemptiveDropEvents"
export PROCESS_SAMPLES_CMD_LNT="${PROCESS_SAMPLES_CMD} --looseNotTight"
