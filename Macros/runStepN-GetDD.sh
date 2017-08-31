#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

BASE_OUT_DIR=/lstore/cms/cbeiraod/Stop4Body/DDEstimate

python getAllDDEstimate.py -i ${TEST_DIR} -o ${BASE_OUT_DIR}
python getAllDDEstimate.py -i ${SWAP_DIR} -o ${BASE_OUT_DIR} --isSwap
python getAllDDEstimate.py -i ${NTUPLE_DIR} -o ${BASE_OUT_DIR} --VR2
python getAllDDEstimate.py -i ${NTUPLE_DIR} -o ${BASE_OUT_DIR} --VR2 --isSpecial
python getAllDDEstimate.py -i ${NTUPLE_DIR} -o ${BASE_OUT_DIR} --VR3
python getAllDDEstimate.py -i ${NTUPLE_DIR} -o ${BASE_OUT_DIR} --VR3 --isSpecial
