#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

if [[ ! -d ${LHESCALE_DIR} ]] ; then
  mkdir -p ${LHESCALE_DIR}
fi

cd LHEWeightScripts

# Submit job for all MC
qsub Step0_GetLHEScale_TTbarJob.sh
qsub Step0_GetLHEScale_TTbarLOJob.sh
qsub Step0_GetLHEScale_TTLepJob.sh
qsub Step0_GetLHEScale_TTpowJob.sh
qsub Step0_GetLHEScale_WjetsJob.sh
qsub Step0_GetLHEScale_QCD.sh
qsub Step0_GetLHEScale_ttX.sh
qsub Step0_GetLHEScale_ZInv.sh
qsub Step0_GetLHEScale_VV.sh
qsub Step0_GetLHEScale_SingleTop.sh
qsub Step0_GetLHEScale_DY.sh

# Submit job for all Data
#qsub Step0_GetLHEScale_DataJob.sh
#qsub Step0_GetLHEScale_DataSingleElectronJob.sh
#qsub Step0_GetLHEScale_DataSingleMuonJob.sh

# Submit job for each group of signal points with a given stop mass
qsub Step0_GetLHEScale_Stop250Job.sh
qsub Step0_GetLHEScale_Stop275Job.sh
qsub Step0_GetLHEScale_Stop300Job.sh
qsub Step0_GetLHEScale_Stop325Job.sh
qsub Step0_GetLHEScale_Stop350Job.sh
qsub Step0_GetLHEScale_Stop375Job.sh
qsub Step0_GetLHEScale_Stop400Job.sh
qsub Step0_GetLHEScale_Stop425Job.sh
qsub Step0_GetLHEScale_Stop450Job.sh
qsub Step0_GetLHEScale_Stop475Job.sh
qsub Step0_GetLHEScale_Stop500Job.sh
qsub Step0_GetLHEScale_Stop525Job.sh
qsub Step0_GetLHEScale_Stop550Job.sh
qsub Step0_GetLHEScale_Stop575Job.sh
qsub Step0_GetLHEScale_Stop600Job.sh
qsub Step0_GetLHEScale_Stop625Job.sh
qsub Step0_GetLHEScale_Stop650Job.sh
qsub Step0_GetLHEScale_Stop675Job.sh
qsub Step0_GetLHEScale_Stop700Job.sh
qsub Step0_GetLHEScale_Stop725Job.sh
qsub Step0_GetLHEScale_Stop750Job.sh
qsub Step0_GetLHEScale_Stop775Job.sh
qsub Step0_GetLHEScale_Stop800Job.sh

cd -
