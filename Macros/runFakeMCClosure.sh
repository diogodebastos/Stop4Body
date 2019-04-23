#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

fakeRateClosure --isPseudoData --isHighDeltaM --verbose --doPlots --doMCFakeClosure --json $JSON_PATH/MC2Process.json --inDir ~/LSTORE/Stop4Body/nTuples_v2019-04-02-fakeMCClosure --outDir ~/LSTORE/Stop4Body/nTuples_v2019-04-02-fakeMCClosure/MCClosure --variables variables2017-closure.json --cuts variables2017-closure.json
