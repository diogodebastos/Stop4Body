#!/bin/bash

alias cmsenv='eval `scramv1 runtime -sh`'

cd /exper-sw/cmst3/cmssw/users/cbeiraod/
. setup.sh

cd /exper-sw/cmst3/cmssw/users/cbeiraod/CMSSW_8_0_14/src/
#cmsenv
eval `scramv1 runtime -sh`

cd /exper-sw/cmst3/cmssw/users/cbeiraod/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/

source SyncExercise.sh

