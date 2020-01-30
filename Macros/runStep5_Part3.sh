#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

python rfromDataCards.py -i ${TEST_DIR}
python plotCSV.py -i ${TEST_DIR} -a
