#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

echo "Preparing to split files"

PRE_TEST=/gstore/t3cms/store/user/cbeiraod/Stop4Body/storage/nTuples_v2017-06-05_quickFix_test/
PRE_TRAIN=/gstore/t3cms/store/user/cbeiraod/Stop4Body/storage/nTuples_v2017-06-05_quickFix_train/
python splitTrainingTest.py -i ${NTUPLE_DIR} --previousTestEvents ${PRE_TEST} --previousTrainEvents ${PRE_TRAIN} --submit

echo "Run the following command to check the progress of the output and resubmit failed jobs:"
echo "  python checkJobs.py -o ${NTUPLE_DIR}_train"
