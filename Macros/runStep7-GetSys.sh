#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

BASE_OUT_DIR=/lstore/cms/dbastos/Stop4Body/DDEstimate

python getAllSystematics.py -i ${TEST_DIR} -o Systematics
python getAllSystematics.py -i ${TEST_DIR} -o Systematics --VR2
python getAllSystematics.py -i ${TEST_DIR} -o Systematics --VR3
