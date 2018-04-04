import ROOT
from ROOT import TFile, TH1F, TCanvas

if __name__ == "__main__":
    import sys
    import argparse

    parser = argparse.ArgumentParser(description='Process the command line options')

    parser.add_argument('-f', '--file1', required=True, help='File1 name')
    parser.add_argument('-k', '--file2', required=True, help='File2 name')
    parser.add_argument('-p', '--histogram', required=True, help='Histogram name to be plotted')

    args = parser.parse_args()
    file1Name=args.file1
    file2Name=args.file2
    histName=args.histogram

    f1 = ROOT.TFile(file1Name,"READ")
    f2 = ROOT.TFile(file2Name,"READ")

    h1 = f1.Get(histName)
    h2 = f2.Get(histName)

    h1.SetTitle("H1")
    h2.SetTitle("H2")

    h1.SetLineColor(ROOT.kBlue)
    h2.SetLineColor(ROOT.kRed)


    c = TCanvas("c",histName,800,600)

    h1.Draw()
    h2.Draw("same")

    c.SaveAs("plots/"+histName+".pdf")

#/lstore/cms/cbeiraod/Stop4Body/puWeights
#/lstore/cms/dbastos/Stop4Body/nTuples_v2018-04-03/
#puWeights_stop675.json.root
#python plotStep0.py -f /lstore/cms/dbastos/Stop4Body/nTuples_v2018-04-03/puWeights_stop675.json.root -k /lstore/cms/cbeiraod/Stop4Body/puWeights/puWeights_stop675.json.root -p sample_T2DegStop_675_595_nTrueInt
