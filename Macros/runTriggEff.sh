#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

getTriggerEfficiency --josn ${JSON_PATH}/DataSingleElectron.josn --inDir  --year %{YEAR}