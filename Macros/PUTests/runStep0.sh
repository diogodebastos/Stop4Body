#!/bin/bash

. ../setupJSONs.sh
. ../setupPaths.sh

if [[ ! -d ${PU_TEST_DIR} ]] ; then
  mkdir -p ${PU_TEST_DIR}
fi

# Submit job for ttbar MC
qsub Step0_GetPUWeights_ttbarJob.sh

# Submit job for all Data
qsub Step0_GetPUWeights_DataJob.sh
