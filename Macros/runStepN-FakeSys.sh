#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

BASE_OUT_DIR=/lstore/cms/cbeiraod/Stop4Body/

python getFakeSystematics.py -i ${NTUPLE_DIR} -o ${BASE_OUT_DIR}
