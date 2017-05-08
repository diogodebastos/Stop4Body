import os,subprocess,ROOT
import pickle
from array import array

if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-i', '--inFile', required=True, help='Name of the input file')
  parser.add_argument('-u', '--uncFile', required=True, help='Name of the uncertainty file')
  parser.add_argument('-o', '--outFile', required=True, help='Name of the output file')
  #parser.add_argument('-m', '--isMuon', action='store_true', help='Whether the efficiency is for muons')
  #parser.add_argument('-e', '--isElectron', action='store_true', help='Whether the efficiency is for electrons')

  args = parser.parse_args()

  efficiencies = pickle.load(open(args.inFile, 'rb'))
  uncertainties = pickle.load(open(args.uncFile, 'rb'))

  fout = ROOT.TFile(args.outFile, "RECREATE")

  for lep in ['mu', 'el']:
    if lep == 'el':
      bins = array('d',[0,5,10,20,30,50,80,200,220])
      histoLow = ROOT.TH1F("electronEfficiencyLowEta", "Electron Efficiency (eta < 1.5)", 8, bins)
      histoHigh = ROOT.TH1F("electronEfficiencyHighEta", "Electron Efficiency (eta > 1.5)", 8, bins)

      histoLow.GetXaxis().SetBinLabel(1, str("0_5"))
      histoLow.GetXaxis().SetBinLabel(2, str("ptL"))
      histoLow.GetXaxis().SetBinLabel(3, str("ptM"))
      histoLow.GetXaxis().SetBinLabel(4, str("ptH"))
      histoLow.GetXaxis().SetBinLabel(5, str("30-50"))
      histoLow.GetXaxis().SetBinLabel(6, str("50-80"))
      histoLow.GetXaxis().SetBinLabel(7, str("80-200"))
      histoLow.GetXaxis().SetBinLabel(8, str(">200"))

      histoHigh.GetXaxis().SetBinLabel(1, str("0_5"))
      histoHigh.GetXaxis().SetBinLabel(2, str("ptL"))
      histoHigh.GetXaxis().SetBinLabel(3, str("ptM"))
      histoHigh.GetXaxis().SetBinLabel(4, str("ptH"))
      histoHigh.GetXaxis().SetBinLabel(5, str("30-50"))
      histoHigh.GetXaxis().SetBinLabel(6, str("50-80"))
      histoHigh.GetXaxis().SetBinLabel(7, str("80-200"))
      histoHigh.GetXaxis().SetBinLabel(8, str(">200"))
    else:
      bins = array('d',[0,3.5,5,10,20,30,50,80,200,220])
      histoLow = ROOT.TH1F("muonEfficiencyLowEta", "Muon Efficiency (eta < 1.5)", 9, bins)
      histoHigh = ROOT.TH1F("muonEfficiencyHighEta", "Muon Efficiency (eta > 1.5)", 9, bins)

      histoLow.GetXaxis().SetBinLabel(1, str("0_3p5"))
      histoLow.GetXaxis().SetBinLabel(2, str("ptVL"))
      histoLow.GetXaxis().SetBinLabel(3, str("ptL"))
      histoLow.GetXaxis().SetBinLabel(4, str("ptM"))
      histoLow.GetXaxis().SetBinLabel(5, str("ptH"))
      histoLow.GetXaxis().SetBinLabel(6, str("30-50"))
      histoLow.GetXaxis().SetBinLabel(7, str("50-80"))
      histoLow.GetXaxis().SetBinLabel(8, str("80-200"))
      histoLow.GetXaxis().SetBinLabel(9, str(">200"))

      histoHigh.GetXaxis().SetBinLabel(1, str("0_3p5"))
      histoHigh.GetXaxis().SetBinLabel(2, str("ptVL"))
      histoHigh.GetXaxis().SetBinLabel(3, str("ptL"))
      histoHigh.GetXaxis().SetBinLabel(4, str("ptM"))
      histoHigh.GetXaxis().SetBinLabel(5, str("ptH"))
      histoHigh.GetXaxis().SetBinLabel(6, str("30-50"))
      histoHigh.GetXaxis().SetBinLabel(7, str("50-80"))
      histoHigh.GetXaxis().SetBinLabel(8, str("80-200"))
      histoHigh.GetXaxis().SetBinLabel(9, str(">200"))

    for bin in efficiencies[lep]['eta_lt_1p5'].keys():
      binNum = histoLow.Fill(str(bin), 0);

      histoLow.SetBinContent(binNum, efficiencies[lep]['eta_lt_1p5'][bin]);
      histoLow.SetBinError(binNum, uncertainties[lep]['eta_lt_1p5'][bin]);
      histoHigh.SetBinContent(binNum, efficiencies[lep]['eta_gt_1p5'][bin]);
      histoHigh.SetBinError(binNum, uncertainties[lep]['eta_gt_1p5'][bin]);

    histoLow.Write()
    histoHigh.Write()
