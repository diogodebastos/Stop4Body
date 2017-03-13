#!/bin/bash

. setupJSONs.sh
. setupPaths.sh

if [[ ! -d ${NTUPLE_DIR} ]] ; then
  mkdir -p ${NTUPLE_DIR}
fi
if [[ ! -d ${TEST_DIR} ]] ; then
  mkdir -p ${TEST_DIR}
fi
if [[ ! -d ${TRAIN_DIR} ]] ; then
  mkdir -p ${TRAIN_DIR}
fi

cd Step1Scripts

# Submit job for all MC
qsub Step1_ProcessSamples_backgroundMCJob.sh
qsub Step1_ProcessSamples_altTTbarJob.sh
qsub Step1_ProcessSamples_otherMCJob1.sh
qsub Step1_ProcessSamples_otherMCJob2.sh
qsub Step1_ProcessSamples_otherMCJob3.sh
qsub Step1_ProcessSamples_otherMCJob4.sh

# Submit job for all Data
qsub Step1_ProcessSamples_DataJob.sh

# Submit job for each group of signal points with a given stop mass
qsub Step1_ProcessSamples_Stop250Job.sh
qsub Step1_ProcessSamples_Stop275Job.sh
qsub Step1_ProcessSamples_Stop300Job.sh
qsub Step1_ProcessSamples_Stop325Job.sh
qsub Step1_ProcessSamples_Stop350Job.sh
qsub Step1_ProcessSamples_Stop375Job.sh
qsub Step1_ProcessSamples_Stop400Job.sh
qsub Step1_ProcessSamples_Stop425Job.sh
qsub Step1_ProcessSamples_Stop450Job.sh
qsub Step1_ProcessSamples_Stop475Job.sh
qsub Step1_ProcessSamples_Stop500Job.sh
qsub Step1_ProcessSamples_Stop525Job.sh
qsub Step1_ProcessSamples_Stop550Job.sh
qsub Step1_ProcessSamples_Stop575Job.sh
qsub Step1_ProcessSamples_Stop600Job.sh
qsub Step1_ProcessSamples_Stop625Job.sh
qsub Step1_ProcessSamples_Stop650Job.sh
qsub Step1_ProcessSamples_Stop675Job.sh
qsub Step1_ProcessSamples_Stop700Job.sh
qsub Step1_ProcessSamples_Stop725Job.sh
qsub Step1_ProcessSamples_Stop750Job.sh
qsub Step1_ProcessSamples_Stop775Job.sh
qsub Step1_ProcessSamples_Stop800Job.sh

cd -
