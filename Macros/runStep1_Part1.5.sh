#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

echo "Preparing to split files"

PRE_TEST=
PRE_TRAIN=
python splitTrainingTest.py -i ${NTUPLE_DIR} --previousTestEvents ${PRE_TEST} --previousTrainEvents ${PRE_TRAIN} --submit

echo "Run the following command to check the progress of the output and resubmit failed jobs:"
echo "  python checkJobs.py -o ${NTUPLE_DIR}"
