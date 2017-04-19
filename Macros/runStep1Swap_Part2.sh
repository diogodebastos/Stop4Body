#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

cd Step1SwapScripts

qsub Step1_Part2.sh

cd -
