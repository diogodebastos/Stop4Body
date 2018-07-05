#!/bin/bash

# Info at: https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2016Analysis
# Take from: https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/

###### 2016
#GOLDEN_JSON=https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/ReReco/Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt
#PILEUP_LATEST=https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/PileUp/pileup_latest.txt
MC_PU=https://github.com/HephySusySW/Workspace/raw/80X-master/DegenerateStopAnalysis/python/cmgPostProcessing/pileup/mcSpring16_25ns_pu.root

###### 2017
GOLDEN_JSON=https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/ReReco/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON.txt
PILEUP_LATEST=https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/PileUp/pileup_latest.txt
#[TODO]MC_PU=

#scp cbeiraod@lxplus.cern.ch:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/PileUp/pileup_latest.txt ./
wget ${PILEUP_LATEST} -O pileup_latest.txt
wget ${GOLDEN_JSON} -O GOLDEN_JSON.txt

# Details: https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData
# Vary minBias by 4.6%
pileupCalc.py -i GOLDEN_JSON.txt --inputLumiJSON pileup_latest.txt  --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 DataPileupHistogram.root
pileupCalc.py -i GOLDEN_JSON.txt --inputLumiJSON pileup_latest.txt  --calcMode true --minBiasXsec 72383 --maxPileupBin 100 --numPileupBins 100 DataPileupHistogramUp.root
pileupCalc.py -i GOLDEN_JSON.txt --inputLumiJSON pileup_latest.txt  --calcMode true --minBiasXsec 66017 --maxPileupBin 100 --numPileupBins 100 DataPileupHistogramDown.root

#Deprecated
#TODO: Remove this from code or make it to legacy 
wget ${MC_PU} -O MCPileupHistogram.root