#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

cd Step1Scripts

qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step1_Part2.sh

cd -
