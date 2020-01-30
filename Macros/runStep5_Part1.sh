#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

python getBDTcut.py -i ${TEST_DIR} -o BdtCut
