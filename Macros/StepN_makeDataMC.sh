#!/bin/bash

### NEW CODE TO TEST
#alias cmsenv='eval `scramv1 runtime -sh`'

#export SCRAM_ARCH=slc6_amd64_gcc530
#export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch
#source $VO_CMS_SW_DIR/cmsset_default.sh
#export CMS_PATH=$VO_CMS_SW_DIR
#source /cvmfs/cms.cern.ch/crab3/crab.sh

#cd $CMSSW_BASE/src/
#eval `scramv1 runtime -sh`

#cd /exper-sw/cmst3/cmssw/users/cbeiraod/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/
#cd UserCode/Stop4Body/Macros/
#. setupJSONs.sh
###################
. setupJSONs.sh
. setupPaths.sh

FS=el

INPUT=${NTUPLE_DIR}
#OUTPUT=${INPUT}/DataMC-signal-stack
#OUTPUT=${INPUT}/DataMC-QCD_Mu_en
OUTPUT=${INPUT}/LeptonStudies_${FS}
#OUTPUT=${INPUT}/DataMC-signal-stack-el-cutflow
#OUTPUT=${INPUT}/DataMC-signal-stack-SubJob
OUTPUT_QUICK=${INPUT}/DataMC-quick
OUTPUT_FAKE=${INPUT}/Plots_fake_PU

VARS=variables2017-${FS}Studies_MC.json

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}
  fi
  if [[ ! -d ${OUTPUT_QUICK} ]] ; then
    mkdir -p ${OUTPUT_QUICK}
  fi
#  if [[ ! -d ${OUTPUT_FAKE} ]] ; then
#    mkdir -p ${OUTPUT_FAKE}
#  fi

### Quick and with no SFs
#  makePlots --doSummary --quick --noSF --json ${JSON_PATH}/plot${YEAR}-inj-quick.json  --outDir ${OUTPUT_QUICK} --inDir ${INPUT} --variables variables2017.json --cuts variables2017.json
### Final plots with signal injected
#  makePlots --doSummary         --json ${JSON_PATH}/plot${YEAR}-inj.json        --outDir ${OUTPUT}       --inDir ${INPUT} --variables variables2017.json --cuts variables2017.json
### Plot Lepton Studies: Full MC with 4 SPs and different Lepton cutID
  makePlots --doSummary --json $JSON_PATH/lepStudies.json --outDir ${OUTPUT} --inDir ${INPUT} --variables ${VARS} --cuts ${VARS}
### QCD_Mu_enriched
#  makePlots --doSummary         --json ${JSON_PATH}/plot${YEAR}_QCD_Mu_en.json        --outDir ${OUTPUT}       --inDir ${INPUT} --variables variables2017.json --cuts variables2017.json

 # makePlots --doSummary         --json ${JSON_PATH}/plot${YEAR}-inj.json        --outDir ${OUTPUT}       --inDir ${INPUT} --variables variables2017.json --cuts variables2017.json
  #makePlots --doSummary         --json ${JSON_PATH}/plot${YEAR}-inj.json        --outDir ${OUTPUT}       --inDir ${INPUT} --variables variables2017-mu.json --cuts variables2017-mu.json

# Data/MC for the fake rate method
#  makePlots --quick --doSummary --json ${JSON_PATH}/plot${YEAR}-fake.json --outDir ${OUTPUT_FAKE} --inDir ${INPUT} --variables variables2017-plotFakesTL.json --cuts variables2017-plotFakesTL.json --dofakeratio --noSF
fi
