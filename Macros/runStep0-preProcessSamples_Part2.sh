#!/bin/bash

. setupPaths.sh

hadd -f ${PREPROCESSSAMPLES_DIR}/preProcessSamples.root ${PREPROCESSSAMPLES_DIR}/preProcessSamples_*.root
