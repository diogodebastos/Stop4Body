import os,subprocess,ROOT
import pickle

if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-i', '--inFile', required=True, help='Name of the input file')
  parser.add_argument('-o', '--outFile', required=True, help='Name of the output file')

  args = parser.parse_args()

  filterEffs = pickle.load(open(args.inFile, 'rb'))

  fout = ROOT.TFile(args.outFile, "RECREATE")
  histo = ROOT.TH2F("filterEfficiency", "Filter Efficiency",23,237.5,812.5,128,165,815)

  for stop in filterEffs:
    for neut in filterEffs[stop]:
      histo.Fill(stop, neut, filterEffs[stop][neut])
  histo.Write()
