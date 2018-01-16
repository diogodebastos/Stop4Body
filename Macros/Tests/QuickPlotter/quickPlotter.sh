#!/bin/bash

cd ../..
. setupJSONs.sh
. setupPaths.sh
cd -

INPUT=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19
INPUT_TEST=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-10-19_test
INPUT_SWAP=/lstore/cms/cbeiraod/Stop4Body/nTuples_v2017-08-13_swap
OUTPUT=/lstore/cms/cbeiraod/Stop4Body/ANPlots/

if [[ -d ${INPUT} ]] ; then
  if [[ ! -d ${OUTPUT} ]] ; then
    mkdir -p ${OUTPUT}/DataMC/AR
    mkdir -p ${OUTPUT}/DataMC/LNT
    mkdir -p ${OUTPUT}/DataMC/VR1
    mkdir -p ${OUTPUT}/DataMC/VR2
    mkdir -p ${OUTPUT}/DataMC/VR3
    mkdir -p ${OUTPUT}/Corr/AR
    mkdir -p ${OUTPUT}/Corr/LNT
    mkdir -p ${OUTPUT}/Corr/VR1
    mkdir -p ${OUTPUT}/Corr/VR2
    mkdir -p ${OUTPUT}/Corr/VR3
  fi

  for deltaM in 10 20 30 40 50 60 70 80; do
    mkdir -p ${OUTPUT}/BDT/DeltaM${deltaM}/AR
    mkdir -p ${OUTPUT}/BDT/DeltaM${deltaM}/LNT
    mkdir -p ${OUTPUT}/BDT/DeltaM${deltaM}/VR1
    mkdir -p ${OUTPUT}/BDT/DeltaM${deltaM}/VR2
    mkdir -p ${OUTPUT}/BDT/DeltaM${deltaM}/VR3
  done


  ### Submit the jobs
  qsub ARjob.sh
  #qsub LNTjob.sh
  #qsub VR1job.sh
  qsub VR2job.sh
  qsub VR3job.sh

  for deltaM in 10 20 30 40 50 60 70 80; do
    qsub DM${deltaM}_ARjob.sh
    #qsub DM${deltaM}_LNTjob.sh
    #qsub DM${deltaM}_VR1job.sh
    qsub DM${deltaM}_VR2job.sh
    qsub DM${deltaM}_VR3job.sh
  done
fi
