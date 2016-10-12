#!/bin/bash

SIGNAL=~cbeiraod/local-area/Stop4Body/nTuples_v2016-08-31/T2DegStop_300_270.root
BACKGROUND=~cbeiraod/local-area/Stop4Body/nTuples_v2016-08-31/Background.root

if [[ -f ${SIGNAL} && -f ${BACKGROUND} ]] ; then
  trainMVA --method BDT --signalFile ${SIGNAL} --backgroundFile ${BACKGROUND}
  root runTMVAGui.C
fi

