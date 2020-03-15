#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

#--doPlots

fakeRateClosure --doPlots --isPseudoData --isHighDeltaM --verbose --doMCFakeClosure --json $JSON_PATH/MC2Process.json --inDir ~/LSTORE/Stop4Body/nTuples_v2019-05-06-skimmed --outDir ~/LSTORE/Stop4Body/nTuples_v2019-05-06-skimmed/MCClosure --variables variables2017-closure.json --cuts variables2017-closure.json

#fakeRateClosure --isPseudoData --isHighDeltaM --verbose --doMCFakeClosure --json $JSON_PATH/TTbar.json --inDir ~/LSTORE/Stop4Body/nTuples_v2019-05-10-TTbar-closure --outDir ~/LSTORE/Stop4Body/nTuples_v2019-05-10-TTbar-closure/MCClosure --variables variables2017-closure.json --cuts variables2017-closure.json
#fakeRateClosure --isPseudoData --isHighDeltaM --verbose --doMCFakeClosure --json $JSON_PATH/Wjets.json --inDir ~/LSTORE/Stop4Body/nTuples_v2019-05-10-WJets-closure --outDir ~/LSTORE/Stop4Body/nTuples_v2019-05-10-WJets-closure/MCClosure --variables variables2017-closure.json --cuts variables2017-closure.json
#fakeRateClosure --isPseudoData --isHighDeltaM --verbose --doMCFakeClosure --json $JSON_PATH/ZInv.json --inDir ~/LSTORE/Stop4Body/nTuples_v2019-05-10-ZInv-closure --outDir ~/LSTORE/Stop4Body/nTuples_v2019-05-10-ZInv-closure/MCClosure --variables variables2017-closure.json --cuts variables2017-closure.json
#fakeRateClosure --isPseudoData --isHighDeltaM --verbose --doMCFakeClosure --json $JSON_PATH/ZInv.json --inDir ~/LSTORE/Stop4Body/nTuples_v2019-05-10-ZInv-QCDratio-closure --outDir ~/LSTORE/Stop4Body/nTuples_v2019-05-10-ZInv-QCDratio-closure/MCClosure --variables variables2017-closure.json --cuts variables2017-closure.json
