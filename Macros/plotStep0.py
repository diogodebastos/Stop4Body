import ROOT
from ROOT import TFile, TH1F, TCanvas, TTree, THStack, TText

if __name__ == "__main__":
    import sys
    import argparse

    parser = argparse.ArgumentParser(description='Process the command line options')

    parser.add_argument('-f', '--file1', required=True, help='File1 name')
    parser.add_argument('-k', '--file2', help='File2 name')
    parser.add_argument('-p', '--variable', help='Variable/Histogram name to be plotted')
    parser.add_argument('-y', '--year', action='store_true', help='2016 vs 2017')
    parser.add_argument(      '--pileUp', action='store_true',help='plot pileUp distribution')

    args = parser.parse_args()
    file1Name=args.file1
    file2Name=args.file2
    ROOT.gStyle.SetOptStat("00000000")
#   ROOT.gStyle.SetOptTitle(0)
#   ROOT.gStyle.SetPadTickY(1)
    
    if args.year:
        cbeiraodPath="/lstore/cms/cbeiraod/Stop4Body/puWeights/"
        #dbastosPath="/lstore/cms/dbastos/Stop4Body/nTuples_v2018-04-09/"
        dbastosPath="/lstore/cms/dbastos/Stop4Body/puWeights/2017/"      
        f1 = ROOT.TFile(cbeiraodPath+file1Name,"READ")
        f2 = ROOT.TFile(dbastosPath+file1Name,"READ")
        file2Name=file1Name
        var=args.variable
        h1 = f1.Get(var)
        h2 = f2.Get(var)
        h1.GetXaxis().SetTitle("X axis title")
        h1.GetYaxis().SetTitle("Y axis title")
    elif args.pileUp:
        basePath = "/lstore/cms/dbastos/Stop4Body/"
        nTupleDir = "nTuples_v2018-08-24/"
        var = "nVert"
        
        f1 = ROOT.TFile(basePath+nTupleDir+file1Name,"READ")
        f2 = ROOT.TFile(basePath+nTupleDir+file2Name,"READ")
        t1=f1.Get("bdttree")
        t2=f2.Get("bdttree")
        h1 = TH1F("h1",var,50,0,50)
        h2 = TH1F("h2",var,50,0,50)
        t1.Draw("nVert>>h1","","goff")
        t2.Draw("nVert>>h2","","goff")
        
        
    else:
        f1 = ROOT.TFile(file1Name,"READ")
        f2 = ROOT.TFile(file2Name,"READ")

    c1 = TCanvas("c1",var, 1200, 1350)
    #c1.Range()
    hs= THStack("hs","")
    hs.Add(h1)
    hs.Add(h2)
    
    h1.SetLineColor(ROOT.kBlue)
    h2.SetLineColor(ROOT.kRed)
    h1.SetTitle(file1Name.replace(".root",""))
    h2.SetTitle(file2Name.replace(".root",""))
    
    #h1.Draw()
    #h2.Draw("same")
    hs.Draw()
    
    T = TText()
    T.SetTextFont(42)
    T.SetTextAlign(21)
    T.DrawTextNDC(.5,.95,var)

    c1.BuildLegend(0.66, 0.87, 0.93, 0.77, "")
    c1.SetTitle(var)
    #c.cd()
    #Tl = ROOT.TLatex(0.5,0.9,var)
    #Tl.SetTextSize(0.04)
    #Tl.SetTextAlign(13)
    #Tl.Draw()

    pdfName=var+"_"+file1Name.replace(".root","")+"_vs_"+file2Name.replace(".root","")
    c1.SaveAs("plots/"+pdfName+".pdf")

#/lstore/cms/cbeiraod/Stop4Body/puWeights
#/lstore/cms/dbastos/Stop4Body/nTuples_v2018-04-03/
#puWeights_stop675.json.root
#python plotStep0.py -f /lstore/cms/dbastos/Stop4Body/nTuples_v2018-04-03/puWeights_stop675.json.root -k /lstore/cms/cbeiraod/Stop4Body/puWeights/puWeights_stop675.json.root -p sample_T2DegStop_675_595_nTrueInt
#python plotStep0.py -y -f puWeights_Wjets.json.root -p process_WJets_puWeight
#python plotStep0.py --pileUp -f Data_2017C_MetHT.root -k TTJets.root
