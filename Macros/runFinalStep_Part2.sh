#!/bin/bash

. setupPaths.sh

DATACARD_DIR=../FromPedrame/DataCards/

python getLimits.py -i ${DATACARD_DIR} -o ${LIMIT_DIR} -c -f -u


for LIMIT_NAME in aPrioriAsymp aPosterioriAsymp aPrioriFullCLs aPosterioriFullCLs ; do
python  pklToHistos.py --inputPickle ${LIMIT_DIR}/${LIMIT_NAME}.pkl --outputFile ${LIMIT_DIR}/${LIMIT_NAME}.root
python  smoothHistos.py --inputFile ${LIMIT_DIR}/${LIMIT_NAME}.root --outputFile ${LIMIT_DIR}/${LIMIT_NAME}_smooth.root
sh makeConfigFile.sh  ${LIMIT_DIR}/${LIMIT_NAME}_smooth.root  ${LIMIT_DIR}/${LIMIT_NAME}.cfg
python PlotsSMS/python/makeSMSplots.py ${LIMIT_DIR}/${LIMIT_NAME}.cfg ${LIMIT_NAME} T2DegStop

# Repeat the above, but for the DM plot
python  pklToHistos.py --inputPickle ${LIMIT_DIR}/${LIMIT_NAME}DM.pkl --outputFile ${LIMIT_DIR}/${LIMIT_NAME}DM.root
python  smoothHistos.py --inputFile ${LIMIT_DIR}/${LIMIT_NAME}DM.root --outputFile ${LIMIT_DIR}/${LIMIT_NAME}DM_smooth.root --dmplot
sh makeConfigFile.sh  ${LIMIT_DIR}/${LIMIT_NAME}DM_smooth.root  ${LIMIT_DIR}/${LIMIT_NAME}DM.cfg
python PlotsSMS/python/makeSMSplots.py ${LIMIT_DIR}/${LIMIT_NAME}DM.cfg ${LIMIT_NAME}DM T2DegStop_dm
done

