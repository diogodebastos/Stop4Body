#!/bin/bash

alias cmsenv='eval `scramv1 runtime -sh`'

cd /exper-sw/cmst3/cmssw/users/cbeiraod/
. setup.sh

cd /exper-sw/cmst3/cmssw/users/cbeiraod/Stop4Body/CMSSW_8_0_14/src/
#cd $CMSSW_BASE/src/
eval `scramv1 runtime -sh`

#cd /exper-sw/cmst3/cmssw/users/cbeiraod/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/
cd UserCode/Stop4Body/Macros/

. setupPaths.sh

INPUT=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19
INPUT_TEST=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test
INPUT_SWAP=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-08-13_swap
OUTPUT=/lstore/cms/cbeiraod/Stop4Body/ANPlots/

deltaM=80

if [[ -d ${INPUT} || -L ${INPUT} ]] ; then
  VARIABLE_JSON=variablesAN_BDT_AR.json

  JSONFILE=variablesAN_BDT_LNT.json
  makePlots --json ${JSON_PATH}/plot2016_DM${deltaM}RP.json --outDir ${OUTPUT}/BDT/DeltaM${deltaM}/LNT --inDir ${INPUT}_bdt${deltaM}/ --variables ${VARIABLE_JSON} --cuts ${JSONFILE} --suffix bdt --unblind --final
fi
