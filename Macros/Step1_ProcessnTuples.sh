#!/bin/bash

OUTDIR=~cbeiraod/local-area/Stop4Body/nTuples_v2016-10-20/

if [[ ! -d ${OUTDIR} ]] ; then
  mkdir -p ${OUTDIR}
fi

processSamples --json samples.json --outDir ${OUTDIR}
hadd ${OUTDIR}/Background.root ${OUTDIR}/Wjets_100to200.root ${OUTDIR}/Wjets_200to400.root ${OUTDIR}/Wjets_400to600.root ${OUTDIR}/Wjets_600toInf.root ${OUTDIR}/TTJets.root
