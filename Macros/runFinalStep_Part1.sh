#!/bin/bash

. setupPaths.sh

#export JSON_PATH=/lstore/cms/dbastos/REPOS/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/JSON/2016/
#export NTUPLE_DIR=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt30
#export TRAIN_DIR=${NTUPLE_DIR}_train
#export TEST_DIR=${NTUPLE_DIR}_test

#DATACARD_DIR=../FromPedrame/DataCards/
#DATACARD_DIR=/home/t3cms/bargassa/SW/FourBody/CMSSW_8_0_20/src/UserCode/Stop4Body/Macros/DataCards/
# LIMIT_DIR=/lstore/cms/cbeiraod/Stop4Body/LimitsNew/

DATACARD_DIR=/lstore/cms/dbastos/REPOS/Stop4Body-nanoAOD/CMSSW_8_0_14/src/UserCode/Stop4Body-nanoAOD/Macros/DataCards/${YEAR}
LIMIT_DIR=/lstore/cms/dbastos/Stop4Body/Limits-${YEAR}
#LIMIT_DIR=/lstore/cms/dbastos/Stop4Body/Limits${YEAR}
#LIMIT_DIR=/lstore/cms/dbastos/Stop4Body/dev_Limits${YEAR}
#LIMIT_DIR=/lstore/cms/dbastos/Stop4Body/dev_oldHC_Limits${YEAR}
#LIMIT_DIR=/lstore/cms/dbastos/Stop4Body/Limits_${YEAR}
LIMIT_DIR=/lstore/cms/dbastos/Stop4Body/Limits_${YEAR}_crisSW
LIMIT_DIR=/lstore/cms/dbastos/Stop4Body/Limits_${YEAR}

python getLimits.py -i ${DATACARD_DIR} -o ${LIMIT_DIR} -s -f
#python getLimits.py -i ${DATACARD_DIR} -o ${LIMIT_DIR} -s -f -d
#python getLimits.py -i ${DATACARD_DIR} -o ${LIMIT_DIR} -s -f -u


# Do not forget to retrieve and built the following repository before running the second step
# https://github.com/CMS-SUS-XPAG/PlotsSMS

# git clone git@github.com:CMS-SUS-XPAG/PlotsSMS.git
# cd PlotsSMS
# make
