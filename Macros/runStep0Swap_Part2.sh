#!/bin/bash

. setupPaths.sh

hadd -f ${SWAP_DIR}/puWeights.root ${SWAP_DIR}/puWeights_*.root

