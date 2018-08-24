#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

if [[ ! -d ${LHESCALE_DIR} ]] ; then
  mkdir -p ${LHESCALE_DIR}
fi

cd LHEWeightScripts
if [ ${YEAR} == 2016 ]; then
 # Submit job for all MC
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_TTbarJob.sh
 #qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_TTbarLOJob.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_TTLepJob.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_TTpowJob.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_WjetsJob.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_QCD.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_ttX.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_ZInvJob.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_VV.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_SingleTop.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_DY.sh
 
 # Submit job for all Data
 #qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_DataJob.sh
 #qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_DataSingleElectronJob.sh
 #qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_DataSingleMuonJob.sh
 
 # Submit job for each group of signal points with a given stop mass
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop250Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop275Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop300Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop325Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop350Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop375Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop400Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop425Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop450Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop475Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop500Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop525Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop550Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop575Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop600Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop625Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop650Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop675Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop700Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop725Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop750Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop775Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_Stop800Job.sh
elif [ ${YEAR} == 2017 ]; then
 # Submit job for all MC
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_TTbarJob.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_WjetsJob.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_ZInvJob.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_OtherMC1Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_OtherMC2Job.sh
 # Submit job for all Data
 # qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetLHEScale_DataJob.sh
 # Submit job for each group of signal points with a given stop mass

fi
cd -
