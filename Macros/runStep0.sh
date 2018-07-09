#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

if [[ ! -d ${NTUPLE_DIR} ]] ; then
  mkdir -p ${NTUPLE_DIR}
fi

cd Step0Scripts
if [ ${YEAR} == 2016 ]; then
 # Submit job for all MC
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_TTbarJob.sh
 #qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_TTbarLOJob.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_TTLepJob.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_TTpowJob.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_WjetsJob.sh
 #qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_otherMCJob1.sh
 #qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_otherMCJob2.sh
 #qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_otherMCJob3.sh
 #qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_otherMCJob4.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_QCD.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_ttX.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_ZInv.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_VV.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_SingleTop.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_DY.sh
 
 # Submit job for all Data
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_DataJob.sh
 
 # Submit job for each group of signal points with a given stop mass
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop250Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop275Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop300Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop325Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop350Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop375Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop400Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop425Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop450Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop475Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop500Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop525Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop550Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop575Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop600Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop625Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop650Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop675Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop700Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop725Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop750Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop775Job.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_Stop800Job.sh
elif [ ${YEAR} == 2017 ]; then
 # Submit job for all MC
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_WjetsJob_2017.sh
 qsub -v CMSSW_BASE=$CMSSW_BASE Step0_GetPUWeights_ZInvJob_2017.sh 
fi
cd -

python processFilterEfficiencies.py -i $CMSSW_BASE/src/UserCode/Stop4Body/data/filterEffs_SMS_T2tt_dM_10to80_genHT_160_genMET_80_mWMin_0p1.pkl -o $CMSSW_BASE/src/UserCode/Stop4Body/data/filterEffs_SMS_T2tt_dM_10to80_genHT_160_genMET_80_mWMin_0p1.root
python processLeptonEfficiencies.py -i $CMSSW_BASE/src/UserCode/Stop4Body/data/tightToLooseRatios_MR14_data-EWK_val.pkl -u $CMSSW_BASE/src/UserCode/Stop4Body/data/tightToLooseRatios_MR14_data-EWK_err.pkl -o $CMSSW_BASE/src/UserCode/Stop4Body/data/tightToLooseRatios.root
