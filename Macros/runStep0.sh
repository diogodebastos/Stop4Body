#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

if [[ ! -d ${NTUPLE_DIR} ]] ; then
  mkdir -p ${NTUPLE_DIR}
fi

cd Step0Scripts

# Submit job for all MC
qsub Step0_GetPUWeights_TTbarJob.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_TTbarLOJob.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_TTLepJob.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_TTpowJob.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_WjetsJob.sh -ac CMSSW_BASE=$CMSSW_BASE
#qsub Step0_GetPUWeights_otherMCJob1.sh -ac CMSSW_BASE=$CMSSW_BASE
#qsub Step0_GetPUWeights_otherMCJob2.sh -ac CMSSW_BASE=$CMSSW_BASE
#qsub Step0_GetPUWeights_otherMCJob3.sh -ac CMSSW_BASE=$CMSSW_BASE
#qsub Step0_GetPUWeights_otherMCJob4.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_QCD.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_ttX.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_ZInv.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_VV.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_SingleTop.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_DY.sh -ac CMSSW_BASE=$CMSSW_BASE

# Submit job for all Data
qsub Step0_GetPUWeights_DataJob.sh -ac CMSSW_BASE=$CMSSW_BASE

# Submit job for each group of signal points with a given stop mass
qsub Step0_GetPUWeights_Stop250Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop275Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop300Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop325Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop350Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop375Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop400Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop425Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop450Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop475Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop500Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop525Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop550Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop575Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop600Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop625Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop650Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop675Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop700Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop725Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop750Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop775Job.sh -ac CMSSW_BASE=$CMSSW_BASE
qsub Step0_GetPUWeights_Stop800Job.sh -ac CMSSW_BASE=$CMSSW_BASE

cd -

python processFilterEfficiencies.py -i $CMSSW_BASE/src/UserCode/Stop4Body/data/filterEffs_SMS_T2tt_dM_10to80_genHT_160_genMET_80_mWMin_0p1.pkl -o $CMSSW_BASE/src/UserCode/Stop4Body/data/filterEffs_SMS_T2tt_dM_10to80_genHT_160_genMET_80_mWMin_0p1.root
python processLeptonEfficiencies.py -i $CMSSW_BASE/src/UserCode/Stop4Body/data/tightToLooseRatios_MR14_data-EWK_val.pkl -u $CMSSW_BASE/src/UserCode/Stop4Body/data/tightToLooseRatios_MR14_data-EWK_err.pkl -o $CMSSW_BASE/src/UserCode/Stop4Body/data/tightToLooseRatios.root
