#!/bin/bash

. ../setupJSONs.sh
. ../setupPaths.sh

if [[ ! -d ${PU_TEST_DIR} ]] ; then
  mkdir -p ${PU_TEST_DIR}
fi


# Submit job for ttbar MC
qsub Step1_ProcessSamples_ttbarJob.sh

# Submit job for all Data
qsub Step1_ProcessSamples_DataJob.sh
