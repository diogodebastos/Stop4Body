#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

if [[ ! -d ${LNT_DIR} ]] ; then
  mkdir -p ${LNT_DIR}
fi

cp ${NTUPLE_DIR}/puWeights.root ${LNT_DIR}/puWeights.root

cd Step1LNTScripts

# Submit job for all MC
qsub Step1_ProcessSamples_TTbarJob.sh
qsub Step1_ProcessSamples_TTbarLOJob.sh
qsub Step1_ProcessSamples_TTLepJob.sh
qsub Step1_ProcessSamples_TTpowJob.sh
qsub Step1_ProcessSamples_Wjets_70to100Job.sh
qsub Step1_ProcessSamples_Wjets_100to200Job.sh
qsub Step1_ProcessSamples_Wjets_200to400Job.sh
qsub Step1_ProcessSamples_Wjets_400to600Job.sh
qsub Step1_ProcessSamples_Wjets_600to800Job.sh
qsub Step1_ProcessSamples_Wjets_800to1200Job.sh
qsub Step1_ProcessSamples_Wjets_1200to2500Job.sh
qsub Step1_ProcessSamples_Wjets_2500toInfJob.sh
qsub Step1_ProcessSamples_otherMCJob1.sh
qsub Step1_ProcessSamples_otherMCJob2.sh
qsub Step1_ProcessSamples_otherMCJob3.sh
qsub Step1_ProcessSamples_otherMCJob4.sh

# Submit job for all Data
qsub Step1_ProcessSamples_DataJob.sh

cd -
