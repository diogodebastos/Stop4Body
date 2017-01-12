#!/bin/bash

# Info at: https://twiki.cern.ch/twiki/bin/view/CMS/PdmV2016Analysis
# Take from: https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/
GOLDEN_JSON=https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/Cert_271036-277933_13TeV_PromptReco_Collisions16_JSON.txt
PILEUP_LATEST=https://cms-service-dqm.web.cern.ch/cms-service-dqm/CAF/certification/Collisions16/13TeV/PileUp/pileup_latest.txt

#scp cbeiraod@lxplus.cern.ch:/afs/cern.ch/cms/CAF/CMSCOMM/COMM_DQM/certification/Collisions16/13TeV/PileUp/pileup_latest.txt ./
wget ${PILEUP_LATEST} -O pileup_latest.txt
wget ${GOLDEN_JSON} -O GOLDEN_JSON.txt

# Details: https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData
pileupCalc.py -i GOLDEN_JSON.txt --inputLumiJSON pileup_latest.txt  --calcMode true --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 DataPileupHistogram.root
#pileupCalc.py -i GOLDEN_JSON.txt --inputLumiJSON pileup_latest.txt  --calcMode observed --minBiasXsec 69200 --maxPileupBin 100 --numPileupBins 100 DataPileupHistogram.root
