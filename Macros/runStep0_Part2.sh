#!/bin/bash

. setupPaths.sh

hadd -f ${NTUPLE_DIR}/puWeights.root ${NTUPLE_DIR}/puWeights_*.root

