#!/bin/bash

alias cmsenv='eval `scramv1 runtime -sh`'

cd /exper-sw/cmst3/cmssw/users/cbeiraod/
. setup.sh

cd /exper-sw/cmst3/cmssw/users/cbeiraod/CMSSW_8_0_14/src/
#cmsenv
eval `scramv1 runtime -sh`

cd /exper-sw/cmst3/cmssw/users/cbeiraod/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/

SIGNAL=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2016-08-31/T2DegStop_300_270.root
BACKGROUND=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2016-08-31/Background.root

if [[ -f ${SIGNAL} && -f ${BACKGROUND} ]] ; then
  trainMVA --method NN --signalFile ${SIGNAL} --backgroundFile ${BACKGROUND}
fi

