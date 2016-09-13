#!/bin/bash

OUTDIR=~/local-area/Stop4Body/New/
OUTDIR=~/working/Stop/Samples/

if [[ ! -d ${OUTDIR} ]] ; then
  mkdir -p ${OUTDIR}
fi

processSamples --json samples.json --outDir ${OUTDIR}
hadd ${OUTDIR}/Background_bdt.root ${OUTDIR}/Wjets_100to200_bdt.root ${OUTDIR}/Wjets_200to400_bdt.root ${OUTDIR}/Wjets_400to600_bdt.root ${OUTDIR}/Wjets_600toInf_bdt.root ${OUTDIR}/TTJets_LO_bdt.root
