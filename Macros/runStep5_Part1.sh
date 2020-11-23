#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

python getBDTcut.py -i ${TEST_DIR} -o BdtCut
# Optimize variable cut
#python getBDTcut.py -i ${TEST_DIR}_${CURRENT_BDT}_HT${HTCUT}_Jet1Pt${JET1PTCUT} -o BdtCut
#python getBDTcut.py -i ${TEST_DIR}_${CURRENT_BDT} -o BdtCut
