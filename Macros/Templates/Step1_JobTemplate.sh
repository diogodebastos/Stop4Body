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
cd UserCode/Stop4Body/Macros/

. setupPaths.sh
JSONFILE=!!OUTPUT_DIRECTORY!!/!!JSON_FILE!!
OUTDIR=!!OUTPUT_DIRECTORY!!
#OUTDIR=./
#cd !!OUTPUT_DIRECTORY!!
MY_PROCESS_SAMPLES_CMD="$PROCESS_SAMPLES_CMD --json $JSONFILE --lheScaleDir $LHESCALE_DIR --outDir $OUTDIR --part !!PART!! !!DO_SWAP!!"
eval $MY_PROCESS_SAMPLES_CMD
