#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

mkdir EfficiencyMaps

INPUT_DIRECTORY=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test_bdt
JSON_FILE=allSamples.json



BDT=10
CUT=0.31
getEfficiencyMap --inDir ${INPUT_DIRECTORY}${BDT}/ --signalRegionCut ${CUT} --suffix bdt --json ${JSON_PATH}/${JSON_FILE} --outFile EfficiencyMaps/map_bdt${BDT}.root

BDT=20
CUT=0.39
getEfficiencyMap --inDir ${INPUT_DIRECTORY}${BDT}/ --signalRegionCut ${CUT} --suffix bdt --json ${JSON_PATH}/${JSON_FILE} --outFile EfficiencyMaps/map_bdt${BDT}.root

BDT=30
CUT=0.47
getEfficiencyMap --inDir ${INPUT_DIRECTORY}${BDT}/ --signalRegionCut ${CUT} --suffix bdt --json ${JSON_PATH}/${JSON_FILE} --outFile EfficiencyMaps/map_bdt${BDT}.root

BDT=40
CUT=0.48
getEfficiencyMap --inDir ${INPUT_DIRECTORY}${BDT}/ --signalRegionCut ${CUT} --suffix bdt --json ${JSON_PATH}/${JSON_FILE} --outFile EfficiencyMaps/map_bdt${BDT}.root

BDT=50
CUT=0.45
getEfficiencyMap --inDir ${INPUT_DIRECTORY}${BDT}/ --signalRegionCut ${CUT} --suffix bdt --json ${JSON_PATH}/${JSON_FILE} --outFile EfficiencyMaps/map_bdt${BDT}.root

BDT=60
CUT=0.50
getEfficiencyMap --inDir ${INPUT_DIRECTORY}${BDT}/ --signalRegionCut ${CUT} --suffix bdt --json ${JSON_PATH}/${JSON_FILE} --outFile EfficiencyMaps/map_bdt${BDT}.root

BDT=70
CUT=0.46
getEfficiencyMap --inDir ${INPUT_DIRECTORY}${BDT}/ --signalRegionCut ${CUT} --suffix bdt --json ${JSON_PATH}/${JSON_FILE} --outFile EfficiencyMaps/map_bdt${BDT}.root

BDT=80
CUT=0.44
getEfficiencyMap --inDir ${INPUT_DIRECTORY}${BDT}/ --signalRegionCut ${CUT} --suffix bdt --json ${JSON_PATH}/${JSON_FILE} --outFile EfficiencyMaps/map_bdt${BDT}.root

root mergeEfficiencyMaps.C
