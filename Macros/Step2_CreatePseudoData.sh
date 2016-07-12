#!/bin/bash

if [[ -d ~/local-area/Stop4Body/New ]] ; then
  makePseudoData --json Macros/samples.json --inDir ~/local-area/Stop4Body/New/ --outDir ~/local-area/Stop4Body/New/ --lumi 5000 --noPresel
fi

