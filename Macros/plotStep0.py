import ROOT
from ROOT import TFile, TH1F, TCanvas

if __name__ == "__main__":
    import sys
    import argparse

    parser = argparse.ArgumentParser(description='Process the command line options')

    parser.add_argument('-f', '--file1', required=True, help='File1 name')
    parser.add_argument('-k', '--file2', help='File2 name')
    parser.add_argument('-p', '--histogram', help='Histogram name to be plotted')
    parser.add_argument('-u', '--unified', action='store_true', help='Give only the name of the file and the hist')
    parser.add_argument(      '--pileUp', action='store_true',help='plot pileUp distribution')

    args = parser.parse_args()
    file1Name=args.file1
    file2Name=args.file2
    histName=args.histogram

    ROOT.gStyle.SetOptStat("00000000")
#    ROOT.gStyle.SetOptTitle(0)

    c = TCanvas("c",histName,800,600)
    
    if args.unified:
        cbeiraodPath="/lstore/cms/cbeiraod/Stop4Body/puWeights/"
        #dbastosPath="/lstore/cms/dbastos/Stop4Body/nTuples_v2018-04-09/"
        dbastosPath="/lstore/cms/dbastos/Stop4Body/puWeights/2017/"      

        f1 = ROOT.TFile(cbeiraodPath+file1Name,"READ")
        f2 = ROOT.TFile(dbastosPath+file1Name,"READ")
    elif args.pileUp:
        basePath = "/lstore/cms/dbastos/Stop4Body/"
        nTupleDir = "nTuples_v2018-08-24/"
        f1 = ROOT.TFile(basePath+nTupleDir+file1Name,"READ")
        f2 = ROOT.TFile(basePath+nTupleDir+file2Name,"READ")
        histName = "nVert"
    else:
        f1 = ROOT.TFile(file1Name,"READ")
        f2 = ROOT.TFile(file2Name,"READ")

    h1 = f1.Get(histName)
    h2 = f2.Get(histName)

    h1.SetLineColor(ROOT.kBlue)
    h2.SetLineColor(ROOT.kRed)

    h1.Draw()
    h2.Draw("same")

    h1.SetTitle(file1Name)
    h1.GetXaxis().SetTitle("X axis title");
    h1.GetYaxis()->SetTitle("Y axis title");
    h2.SetTitle(file2Name")
    
    c.BuildLegend(0.9,0.8,1,0.9)
    h1.SetTitle(histName)
    #c.cd()
    #Tl = ROOT.TLatex(0.5,0.9,histName)
    #Tl.SetTextSize(0.04)
    #Tl.SetTextAlign(13)
    #Tl.Draw()


    pdfName=file1Name+"_"+file2Name+"_"+histName
    c.SaveAs("plots/"+pdfName+".pdf")

#/lstore/cms/cbeiraod/Stop4Body/puWeights
#/lstore/cms/dbastos/Stop4Body/nTuples_v2018-04-03/
#puWeights_stop675.json.root
#python plotStep0.py -f /lstore/cms/dbastos/Stop4Body/nTuples_v2018-04-03/puWeights_stop675.json.root -k /lstore/cms/cbeiraod/Stop4Body/puWeights/puWeights_stop675.json.root -p sample_T2DegStop_675_595_nTrueInt
#python plotStep0.py -u -f puWeights_Wjets.json.root -p process_WJets_puWeight
#python plotStep0.py --pileUp -f Data_2017C_MetHT.root -k TTJets.root