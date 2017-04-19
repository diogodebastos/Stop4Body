#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

if [[ ! -d ${SWAP_DIR} ]] ; then
  mkdir -p ${SWAP_DIR}
fi

cd Step1SwapScripts

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
qsub Step1_ProcessSamples_DataSingleElectronJob.sh
qsub Step1_ProcessSamples_DataSingleMuonJob.sh

# Submit job for each group of signal points with a given stop mass
qsub Step1_ProcessSamples_Stop250Job.sh
qsub Step1_ProcessSamples_Stop275Job.sh
qsub Step1_ProcessSamples_Stop300Job.sh
qsub Step1_ProcessSamples_Stop325Job.sh
qsub Step1_ProcessSamples_Stop350Job.sh
qsub Step1_ProcessSamples_Stop375Job.sh
qsub Step1_ProcessSamples_Stop400Job.sh
qsub Step1_ProcessSamples_Stop425Job.sh
qsub Step1_ProcessSamples_Stop450Job.sh
qsub Step1_ProcessSamples_Stop475Job.sh
qsub Step1_ProcessSamples_Stop500Job.sh
qsub Step1_ProcessSamples_Stop525Job.sh
qsub Step1_ProcessSamples_Stop550Job.sh
qsub Step1_ProcessSamples_Stop575Job.sh
qsub Step1_ProcessSamples_Stop600Job.sh
qsub Step1_ProcessSamples_Stop625Job.sh
qsub Step1_ProcessSamples_Stop650Job.sh
qsub Step1_ProcessSamples_Stop675Job.sh
qsub Step1_ProcessSamples_Stop700Job.sh
qsub Step1_ProcessSamples_Stop725Job.sh
qsub Step1_ProcessSamples_Stop750Job.sh
qsub Step1_ProcessSamples_Stop775Job.sh
qsub Step1_ProcessSamples_Stop800Job.sh

cd -
