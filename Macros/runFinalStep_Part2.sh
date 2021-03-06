#!/bin/bash

. setupPaths.sh

#export JSON_PATH=/lstore/cms/dbastos/REPOS/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/JSON/2016/
#export NTUPLE_DIR=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt30
#export TRAIN_DIR=${NTUPLE_DIR}_train
#export TEST_DIR=${NTUPLE_DIR}_test

#DATACARD_DIR=../FromPedrame/DataCards/
#DATACARD_DIR=/home/t3cms/bargassa/SW/FourBody/CMSSW_8_0_20/src/UserCode/Stop4Body/Macros/DataCards/
#  LIMIT_DIR=/lstore/cms/cbeiraod/Stop4Body/LimitsNew/
# LIMIT_DIR=${BASE_NTUPLE_PATH}/LimitsFinalStep2/

#YEAR=2017	

DATACARD_DIR=/lstore/cms/dbastos/REPOS/Stop4Body-nanoAOD/CMSSW_8_0_14/src/UserCode/Stop4Body-nanoAOD/Macros/DataCards/${YEAR}
#DATACARD_DIR=/lstore/cms/dbastos/REPOS/Stop4Body-nanoAOD/CMSSW_8_0_14/src/UserCode/Stop4Body-nanoAOD/Macros/DataCards/2017
LIMIT_DIR=/lstore/cms/dbastos/Stop4Body/Limits-${YEAR}
#LIMIT_DIR=/lstore/cms/dbastos/Stop4Body/Limits${YEAR}
LIMIT_DIR=/lstore/cms/dbastos/Stop4Body/Limits_${YEAR}_crisSW
#LIMIT_DIR=/lstore/cms/dbastos/Stop4Body/Limits_2017_crisSW
#SW_PLOTS_DIR=/lstore/cms/dbastos/REPOS/Stop4Body-nanoAOD/CMSSW_8_0_14/src/UserCode/Stop4Body-nanoAOD/Macros/
SW_PLOTS_DIR=/exper-sw/cmst3/cmssw/users/cbeiraod/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/

python getLimits.py -i ${DATACARD_DIR} -o ${LIMIT_DIR} -c -f
#python getLimits.py -i ${DATACARD_DIR} -o ${LIMIT_DIR} -c -f -u


for LIMIT_NAME in aPrioriAsymp aPrioriFullCLs ; do
#for LIMIT_NAME in aPrioriAsymp aPosterioriAsymp aPrioriFullCLs aPosterioriFullCLs ; do
  #python  pklToHistos.py --inputPickle ${LIMIT_DIR}/${LIMIT_NAME}.pkl --outputFile ${LIMIT_DIR}/${LIMIT_NAME}.root
  #python  smoothHistos.py --inputFile ${LIMIT_DIR}/${LIMIT_NAME}.root --outputFile ${LIMIT_DIR}/${LIMIT_NAME}_smooth.root
  #if [[ $LIMIT_NAME == aPriori* ]]; then
  #  sh makePrioriConfigFile_${YEAR}.sh  ${LIMIT_DIR}/${LIMIT_NAME}_smooth.root  ${LIMIT_DIR}/${LIMIT_NAME}.cfg
  #else
  #  sh makeConfigFile_${YEAR}.sh  ${LIMIT_DIR}/${LIMIT_NAME}_smooth.root  ${LIMIT_DIR}/${LIMIT_NAME}.cfg
  #fi
  #python ${SW_PLOTS_DIR}/PlotsSMS/python/makeSMSplots.py ${LIMIT_DIR}/${LIMIT_NAME}.cfg ${LIMIT_NAME} T2DegStop

  # Repeat the above, but for the DM plot
  python  pklToHistos.py --inputPickle ${LIMIT_DIR}/${LIMIT_NAME}DM.pkl --outputFile ${LIMIT_DIR}/${LIMIT_NAME}DM.root
  python  smoothHistos.py --inputFile ${LIMIT_DIR}/${LIMIT_NAME}DM.root --outputFile ${LIMIT_DIR}/${LIMIT_NAME}DM_smooth.root --dmplot
  if [[ $LIMIT_NAME == aPriori* ]]; then
    sh makePrioriConfigFile_${YEAR}.sh  ${LIMIT_DIR}/${LIMIT_NAME}DM_smooth.root  ${LIMIT_DIR}/${LIMIT_NAME}DM.cfg
  else
    sh makeConfigFile_${YEAR}.sh  ${LIMIT_DIR}/${LIMIT_NAME}DM_smooth.root  ${LIMIT_DIR}/${LIMIT_NAME}DM.cfg
  fi
  python ${SW_PLOTS_DIR}/PlotsSMS/python/makeSMSplots.py ${LIMIT_DIR}/${LIMIT_NAME}DM.cfg ${LIMIT_NAME}_${YEAR}_DM T2DegStop_dm
done

