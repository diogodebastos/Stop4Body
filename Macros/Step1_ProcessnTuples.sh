#!/bin/bash

OUTDIR=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2016-11-07/
#OUTDIR=/lstore/cms/cbeiraod/Stop4Body/nTuples_synch/

if [[ ! -d ${OUTDIR} ]] ; then
  mkdir -p ${OUTDIR}
fi

#processSamples --json synch.json --outDir ${OUTDIR}
processSamples --json samples.json --outDir ${OUTDIR}
#processSamples --json deltaM.json --outDir ${OUTDIR}
hadd ${OUTDIR}/Background.root ${OUTDIR}/Wjets_100to200.root ${OUTDIR}/Wjets_200to400.root ${OUTDIR}/Wjets_400to600.root ${OUTDIR}/Wjets_600to800.root ${OUTDIR}/Wjets_800to1200.root ${OUTDIR}/Wjets_1200to2500.root ${OUTDIR}/Wjets_2500toInf.root ${OUTDIR}/TTJets.root
