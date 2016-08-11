#!/bin/bash

SIGNAL=~/local-area/Stop4Body/LepFix/T2DegStop_300_270_bdt.root
BACKGROUND=~/local-area/Stop4Body/LepFix/Bckg_bdt.root

if [[ -f ${SIGNAL} && -f ${BACKGROUND} ]] ; then
  trainMVA --method BDT --signalFile ${SIGNAL} --backgroundFile ${BACKGROUND}
#  root runTMVAGui.C
  #DIR=$(pwd)
  #echo "Changing to TMVAGui directory"
  #cd TMVAGui
  #echo "Running TMVAGui.C"
  #root TMVAGui.C\(\"${DIR}/TMVA.root\"\)
  #echo "Returning to starting directory"
  #cd $DIR
fi

