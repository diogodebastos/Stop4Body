#!/bin/bash

INPUT=~/local-area/Stop4Body/New/
OUTPUT=~/local-area/Stop4Body/New/
INPUT=~/local-area/Stop4Body/LepFix/
OUTPUT=~/local-area/Stop4Body/LepFix/

if [[ -d ~/local-area/Stop4Body/New ]] ; then
  makePseudoData --json samplesInj.json --inDir ${INPUT} --outDir ${OUTPUT} --lumi 5000 --noPresel --injectSignal
  mv ${OUTPUT}/PseudoData_bdt.root ${OUTPUT}/PseudoData_Injected_bdt.root
  makePseudoData --json samples.json --inDir ${INPUT} --outDir ${OUTPUT} --lumi 5000 --noPresel
fi

