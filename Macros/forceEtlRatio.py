import os
import math
import ROOT
from ROOT import TFile, TH1D, TCanvas, TTree, THStack, TText

#c1 = TCanvas("c1","tightToLoose ratio - muon", 1600, 900)
#ROOT.gStyle.SetOptStat("00000000")

path="/home/t3cms/dbastos/LSTORE/REPOS/Stop4Body/CMSSW_8_0_14/src/UserCode/Stop4Body/Macros/FakeRate/"
eTL="tightToLooseRatios_2017.root"

os.chdir(path)

file = ROOT.TFile(eTL,"READ")
newFile = ROOT.TFile("tightToLooseRatios_2017_muon_forced.root","RECREATE")
tAllEta = file.Get("tightToLooseRatios_2017_muon_LepPt")
tLowEta = file.Get("tightToLooseRatios_2017_muon_LepPt_LowEta")
tHighEta = file.Get("tightToLooseRatios_2017_muon_LepPt_HighEta")

# Bin 6 at Low Eta
Bin5Low = tLowEta.GetBinContent(5)
Bin7Low = tLowEta.GetBinContent(7)
newBin6Low = (Bin5Low + Bin7Low)/2.0
tLowEta.SetBinContent(6,newBin6Low)

#errorBin5Low = tLowEta.GetBinError(5)
#errorBin7Low = tLowEta.GetBinError(7)
#errorBin6Low = math.sqrt(0.25*(errorBin5Low**2+errorBin7Low**2))
#tLowEta.SetBinError(6,errorBin6Low)

# Bin 6 at High Eta
meanBin6 = (tAllEta.GetBinContent(5)+tAllEta.GetBinContent(7))/2.0
tAllEta.SetBinContent(6, meanBin6)
newBin6High = meanBin6*2 - newBin6Low
tHighEta.SetBinContent(6,newBin6High)

errorBin6All = tAllEta.GetBinError(6)
errorBin6Low = tLowEta.GetBinError(6)
errorBin6High = math.sqrt(4*errorBin6All**2-errorBin6Low**2)
tHighEta.SetBinError(6,errorBin6High)

# Bin 7 at High Eta
Bin7All = tAllEta.GetBinContent(7)
Bin7Low = tLowEta.GetBinContent(7)
newBin7High = Bin7All*2 - Bin7Low
tHighEta.SetBinContent(7,newBin7High)

errorBin7All = tAllEta.GetBinError(7)
errorBin7Low = tLowEta.GetBinError(7)
errorBin7High = math.sqrt(4*errorBin7All**2-errorBin7Low**2)
tHighEta.SetBinError(7,errorBin6High)

tAllEta.Write()
tLowEta.Write()
tHighEta.Write()

newFile.Close()

# Force new bins on eTL
os.system('rm -f tightToLooseRatios_2017.root tightToLooseRatios_2017_muon_LepPt*.root')
os.system('hadd -f tightToLooseRatios_2017.root *tightToLoose*.root')
