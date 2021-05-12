#!/bin/bash

#. setupJSONs.sh
. setupPaths.sh

getTriggerEfficiency --json ${JSON_PATH}/DataSingleElectron.json --inDir ${NTUPLE_DIR} --year ${YEAR} --vars variables_triggerEff.json --cuts variables_triggerEff.json
