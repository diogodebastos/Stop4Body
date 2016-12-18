#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

cd Step1Scripts

qsub Step1_Part2.sh

cd -
