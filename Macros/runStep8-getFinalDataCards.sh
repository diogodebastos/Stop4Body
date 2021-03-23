#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

python getFinalDataCards.py -i ${TEST_DIR} -o DataCards # -y ${YEAR}