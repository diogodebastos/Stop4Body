#!/bin/bash

OUTDIR=/lstore/cms/cbeiraod/Stop4Body/nTuples_synch/
OUTDIR=/lstore/cms/cbeiraod/Stop4Body/nTuples_synchtest/

if [[ ! -d ${OUTDIR} ]] ; then
  mkdir -p ${OUTDIR}
fi

#processSamples --json test.json --outDir ${OUTDIR} --doSync 300 --noSkim
#processSamples --json synch.json --outDir ${OUTDIR} --doSync 300 --noSkim
processSamples --json JSON/synch.json --outDir ${OUTDIR}              --noSkim
