#!/bin/bash

. setupPaths.sh

DATACARD_DIR=../FromPedrame/DataCards/
DATACARD_DIR=/home/t3cms/bargassa/SW/FourBody/CMSSW_8_0_20/src/UserCode/Stop4Body/Macros/DataCards/
LIMIT_DIR=/lstore/cms/cbeiraod/Stop4Body/LimitsNew/

python getLimits.py -i ${DATACARD_DIR} -o ${LIMIT_DIR} -s -f -u


# Do not forget to retrieve and built the following repository before running the second step
# https://github.com/CMS-SUS-XPAG/PlotsSMS

# git clone git@github.com:CMS-SUS-XPAG/PlotsSMS.git
# cd PlotsSMS
# make
