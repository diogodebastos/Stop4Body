#!/bin/bash

alias cmsenv='eval `scramv1 runtime -sh`'

export SCRAM_ARCH=slc6_amd64_gcc530
export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
source $VO_CMS_SW_DIR/cmsset_default.sh
export CMS_PATH=$VO_CMS_SW_DIR
source /cvmfs/cms.cern.ch/crab3/crab.sh

cd $CMSSW_BASE/src/
eval `scramv1 runtime -sh`

#cd /exper-sw/cmst3/cmssw/users/cbeiraod/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/
cd UserCode/Stop4Body-nanoAOD/Macros/

. setupPaths.sh
JSONFILE=!!OUTPUT_DIRECTORY!!/!!JSON_FILE!!
OUTDIR=!!OUTPUT_DIRECTORY!!
#OUTDIR=./
#cd !!OUTPUT_DIRECTORY!!

# Default process samples command
MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --preemptiveDropEvents --doPromptTagging --json $JSONFILE --lheScaleDir $LHESCALE_DIR --outDir $OUTDIR --part !!PART!! !!DO_SWAP!!"
#MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --preemptiveDropEvents --doPromptTagging --jetPtThreshold 15 --json $JSONFILE --lheScaleDir $LHESCALE_DIR --outDir $OUTDIR --part !!PART!! !!DO_SWAP!!"
#MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --preemptiveDropEvents --doPromptTagging --jetPtThreshold 45 --json $JSONFILE --lheScaleDir $LHESCALE_DIR --outDir $OUTDIR --part !!PART!! !!DO_SWAP!!"

#No skim whatsoever
#MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --noSkim --doPromptTagging --json $JSONFILE --lheScaleDir $LHESCALE_DIR --outDir $OUTDIR --part !!PART!! !!DO_SWAP!!"
#MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --noSkim --jetPtThreshold 45 --doPromptTagging --json $JSONFILE --lheScaleDir $LHESCALE_DIR --outDir $OUTDIR --part !!PART!! !!DO_SWAP!!"


# NoSkim
#MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --preemptiveDropEvents --doPromptTagging --noSkim --json $JSONFILE --lheScaleDir $LHESCALE_DIR --outDir $OUTDIR --part !!PART!! !!DO_SWAP!!" 

# No preemptiveDropEvents
#MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --doPromptTagging --json $JSONFILE --lheScaleDir $LHESCALE_DIR --outDir $OUTDIR --part !!PART!! !!DO_SWAP!!"

# Override XS for WJetsHT
#MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --doPromptTagging --overrideXSec --json $JSONFILE --lheScaleDir $LHESCALE_DIR --outDir $OUTDIR --part !!PART!! !!DO_SWAP!!"

# Default process samples command with JetPtThreshold
#MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --doPromptTagging --jetPtThreshold 45 --json $JSONFILE --lheScaleDir $LHESCALE_DIR --outDir $OUTDIR --part !!PART!! !!DO_SWAP!!"

# Closure test
#MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --doFakeClosure --doPromptTagging --json $JSONFILE --lheScaleDir $LHESCALE_DIR --outDir $OUTDIR --part !!PART!! !!DO_SWAP!!"

# No skim for JetHT PD
#MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --doFakeClosure --noSkim --doPromptTagging --doJetHT --json $JSONFILE --lheScaleDir $LHESCALE_DIR --outDir $OUTDIR --part !!PART!! !!DO_SWAP!!"

# Override XS for WJetsHT
#MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --doPromptTagging --overrideXSec --json $JSONFILE --lheScaleDir $LHESCALE_DIR --outDir $OUTDIR --part !!PART!! !!DO_SWAP!!"
#MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --doFakeClosure --doPromptTagging --doJetHT --overrideXSec --json $JSONFILE --lheScaleDir $LHESCALE_DIR --outDir $OUTDIR --part !!PART!! !!DO_SWAP!!"

eval $MY_PROCESS_SAMPLES_CMD
