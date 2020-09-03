#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

if [[ ! -d ${NTUPLE_DIR} ]] ; then
  mkdir -p ${NTUPLE_DIR}
fi

cd Step0PreProcessSamplesScripts

if [ ${YEAR} == 2016 ]; then
 # Submit job for all MC
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_TTbarJob.sh
 #qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_TTbarLOJob.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_TTLepJob.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_TTpowJob.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_WjetsJob.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_QCD.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_ttX.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_ZInvJob.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_VV.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_SingleTop.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_DY.sh

 # Submit job for all Data
 #qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_DataJob.sh
 #qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_DataSingleElectronJob.sh
 #qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_DataSingleMuonJob.sh

 # Submit job for each group of signal points with a given stop mass
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop250Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop275Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop300Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop325Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop350Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop375Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop400Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop425Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop450Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop475Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop500Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop525Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop550Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop575Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop600Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop625Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop650Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop675Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop700Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop725Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop750Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop775Job.sh
 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop800Job.sh
elif [ ${YEAR} == 2017 ] || [ ${YEAR} == 2018 ]; then
 # Test MC samples
 #qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_VV.sh
 # Submit job for all MC
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_TTbarJob.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_WjetsJob.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_ZInvJob.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_OtherMC1Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_OtherMC2Job.sh

 qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_WNJetsJob.sh
 
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_FastSim.sh

 #qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_SingleTop.sh
 #qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_OtherMC2_QCD_Mu_en.sh

 # Submit job for each group of signal points with a given stop mass
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop250Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop275Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop300Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop325Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop350Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop375Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop400Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop425Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop450Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop475Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop500Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop525Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop550Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop575Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop600Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop625Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop650Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop675Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop700Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop725Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop750Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop775Job.sh
# qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step0_preProcessSamples_Stop800Job.sh
fi
cd -

python processFilterEfficiencies.py -i $CMSSW_BASE/src/UserCode/Stop4Body-nanoAOD/data/filterEffs_SMS_T2tt_dM_10to80_genHT_160_genMET_80_mWMin_0p1.pkl -o $CMSSW_BASE/src/UserCode/Stop4Body-nanoAOD/data/filterEffs_SMS_T2tt_dM_10to80_genHT_160_genMET_80_mWMin_0p1.root
python processLeptonEfficiencies.py -i $CMSSW_BASE/src/UserCode/Stop4Body-nanoAOD/data/tightToLooseRatios_MR14_data-EWK_val.pkl -u $CMSSW_BASE/src/UserCode/Stop4Body-nanoAOD/data/tightToLooseRatios_MR14_data-EWK_err.pkl -o $CMSSW_BASE/src/UserCode/Stop4Body-nanoAOD/data/tightToLooseRatios.root
