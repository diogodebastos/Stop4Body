#!/bin/bash

export BASE_PATH=$CMSSW_BASE/src/UserCode/Stop4Body/
export JSON_PATH=$BASE_PATH/Macros/JSON/

export NTUPLE_DIR=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-03-13
export SYNC_DIR=/lstore/cms/cbeiraod/Stop4Body/nTuples_sync
export PU_TEST_DIR=/lstore/cms/cbeiraod/Stop4Body/nTuples_PU_test

export TRAIN_DIR=${NTUPLE_DIR}_train
export TEST_DIR=${NTUPLE_DIR}_test


export PROCESS_SAMPLES_CMD="processSamples --jetPtThreshold 20"
export PROCESS_SAMPLES_CMD="processSamples"
