#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

cd Step1LNTScripts

qsub Step1_Part2.sh

cd -
