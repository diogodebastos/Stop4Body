#!/bin/bash

. setupPaths.sh

hadd -f ${LHESCALE_DIR}/lheWeights.root ${LHESCALE_DIR}/lheWeights_*.root

