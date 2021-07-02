#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

cd Step1Scripts

#qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE Step1_Part2.sh
sbatch -p lipq --mem=2500 Step1_Part2.sh --export=CMSSW_BASE=$CMSSW_BASE

cd -
