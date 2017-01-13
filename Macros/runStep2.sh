#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

cd Step2Scripts

# Submit jobs
qsub Step2_CreatePseudoData.sh
qsub Step2_CreatePseudoData_Inj.sh

cd -
