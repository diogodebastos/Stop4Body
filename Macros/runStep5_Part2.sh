#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

python getXSULforBDTcut.py -i ${TEST_DIR}
