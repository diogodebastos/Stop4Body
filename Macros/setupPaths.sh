#!/bin/bash

source localUserSetup.sh

export YEAR=2017
export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples17_nanoAOD_v2020-08-13
export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples17_nanoAOD_v2020-08-13-newPU
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples17_nanoAOD_v2020-08-13_npvs

export YEAR=2018
export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples18_nanoAOD_v2020-08-13-newPU

export BASE_PATH=$CMSSW_BASE/src/UserCode/Stop4Body-nanoAOD
export MACROS_PATH=$BASE_PATH/Macros
export JSON_PATH=$MACROS_PATH/JSON/${YEAR}

#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples18_nanoAOD_v2020-05-12
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples18_nanoAOD_v2020-05-12-WJets-HT_v7


#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples17_nanoAOD_v2020-05-14
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples17_nanoAOD_v2020-05-14-jetClean
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples17_nanoAOD_v2020-05-14-jetClean-pt45
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples17_nanoAOD_v2020-05-14-jetClean-isTight
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples17_nanoAOD_v2020-05-14-jetClean-atLepGood
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples17_nanoAOD_v2020-05-14-jetClean-atLepGood-WNJets

export TRAIN_DIR=${NTUPLE_DIR}_train
export TEST_DIR=${NTUPLE_DIR}_test

# USED DIRS FOR 2017 AN
#export NTUPLE_DIR=${BASE_NTUPLE_PATH}/nTuples_v2019-06-05-EWK_IRS-low-Mu-SFs

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
export PREPROCESSSAMPLES_DIR=${BASE_NTUPLE_PATH}/preProcessSamples/${YEAR}

export LIMIT_DIR=${BASE_NTUPLE_PATH}/Limits/

#export PROCESS_SAMPLES_CMD="processSamples --jetPtThreshold 20"
#export PROCESS_SAMPLES_CMD="processSamples --preemptiveDropEvents --doPromptTagging --doLooseLeptons --year ${YEAR}"

export PROCESS_SAMPLES_CMD="processSamples --doPromptTagging --doLooseLeptons --year ${YEAR}"
#export PROCESS_SAMPLES_CMD="processSamples --doPromptTagging --year ${YEAR}"
export PROCESS_SAMPLES_CMD_LNT="${PROCESS_SAMPLES_CMD} --looseNotTight"
export PROCESS_SAMPLES_CMD_LOOSE="processSamples --preemptiveDropEvents --doLooseLeptons"
