#!/bin/bash

# Info at: https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2016Analysis
# Take from: https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/

MC_PU=https://github.com/HephySusySW/Workspace/raw/80X-master/DegenerateStopAnalysis/python/cmgPostProcessing/pileup/mcSpring16_25ns_pu.root

MAX_PU=100

if [ ${YEAR} == 2016 ]; then
 GOLDEN_JSON=https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/ReReco/Final/Cert_271036-284044_13TeV_23Sep2016ReReco_Collisions16_JSON.txt
 PILEUP_LATEST=https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/PileUp/pileup_latest.txt
elif [ ${YEAR} == 2017 ]; then
 GOLDEN_JSON=https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/ReReco/Cert_294927-306462_13TeV_EOY2017ReReco_Collisions17_JSON.txt
 PILEUP_LATEST=https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions17/13TeV/PileUp/pileup_latest.txt
elif [ ${YEAR} == 2018 ]; then
 MAX_PU=100
 GOLDEN_JSON=https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/13TeV/PromptReco/Cert_314472-325175_13TeV_PromptReco_Collisions18_JSON.txt
 PILEUP_LATEST=https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions18/13TeV/PileUp/pileup_latest.txt
fi

#scp cbeiraod@lxplus.cern.ch:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/PileUp/pileup_latest.txt ./
wget ${PILEUP_LATEST} -O pileup_latest_${YEAR}.txt
wget ${GOLDEN_JSON} -O GOLDEN_JSON_${YEAR}.txt

# Details: https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData
# Vary minBias by 4.6%
pileupCalc.py -i GOLDEN_JSON_${YEAR}.txt --inputLumiJSON pileup_latest_${YEAR}.txt  --calcMode true --minBiasXsec 69200 --maxPileupBin ${MAX_PU} --numPileupBins ${MAX_PU} DataPileupHistogram_${YEAR}.root
pileupCalc.py -i GOLDEN_JSON_${YEAR}.txt --inputLumiJSON pileup_latest_${YEAR}.txt  --calcMode true --minBiasXsec 72383 --maxPileupBin ${MAX_PU} --numPileupBins ${MAX_PU} DataPileupHistogram_${YEAR}Up.root
pileupCalc.py -i GOLDEN_JSON_${YEAR}.txt --inputLumiJSON pileup_latest_${YEAR}.txt  --calcMode true --minBiasXsec 66017 --maxPileupBin ${MAX_PU} --numPileupBins ${MAX_PU} DataPileupHistogram_${YEAR}Down.root

#Deprecated
#TODO: Remove this from code or make it to legacy
#wget ${MC_PU} -O MCPileupHistogram.root
