#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

# Submit job
qsub Step2_CreatePseudoData.sh
