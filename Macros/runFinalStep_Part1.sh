#!/bin/bash

. setupPaths.sh

DATACARD_DIR=../FromPedrame/DataCards/

python getLimits.py -i ${DATACARD_DIR} -o ${LIMIT_DIR} -s -f -u


# Do not forget to retrieve and built the following repository before running the second step
# https://github.com/CMS-SUS-XPAG/PlotsSMS

# git clone git@github.com:CMS-SUS-XPAG/PlotsSMS.git
# cd PlotsSMS
# make
