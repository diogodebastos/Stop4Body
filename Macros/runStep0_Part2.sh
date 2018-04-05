#!/bin/bash

. setupPaths.sh

hadd -f ${PUWEIGHT_DIR}/puWeights.root ${PUWEIGHT_DIR}/puWeights_*.root

