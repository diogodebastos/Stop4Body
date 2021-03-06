import ROOT
from ROOT import TFile, TH1F, TCanvas, TTree, THStack, TText

nVert="/home/t3cms/dbastos/LSTORE/Stop4Body/nTuples_v2018-09-13/DataMC_PU/Preselection_nVert_syncPlot_nVert.root"

ROOT.gStyle.SetOptStat("00000000")

fNVert = ROOT.TFile(nVert,"READ")
hs= THStack("hs","")
c1 = TCanvas("c1","nVert", 1600, 900)

data = fNVert.Get("Data")
data.Scale(1/data.Integral())
data.SetLineColor(ROOT.kBlack)
hs.Add(data)

ttbar = fNVert.Get("ttbar")
ttbar.Scale(1/ttbar.Integral())
ttbar.SetLineColor(ROOT.kRed)
hs.Add(ttbar)

WJets = fNVert.Get("WJets")
WJets.Scale(1/WJets.Integral())
WJets.SetLineColor(ROOT.kBlue)
hs.Add(WJets)

ZInv = fNVert.Get("ZInv")
ZInv.Scale(1/ZInv.Integral())
ZInv.SetLineColor(ROOT.kGreen)
hs.Add(ZInv)

VV = fNVert.Get("VV")
VV.Scale(1/VV.Integral())
VV.SetLineColor(ROOT.kYellow)
hs.Add(VV)

DYJets = fNVert.Get("DYJets")
DYJets.Scale(1/DYJets.Integral())
DYJets.SetLineColor(ROOT.kMagenta)
hs.Add(DYJets)

SingleTop = fNVert.Get("SingleTop")
SingleTop.Scale(1/SingleTop.Integral())
SingleTop.SetLineColor(ROOT.kCyan)
hs.Add(SingleTop)

QCD = fNVert.Get("QCD")
QCD.Scale(1/QCD.Integral())
QCD.SetLineColor(ROOT.kOrange)
hs.Add(QCD)

ttx = fNVert.Get("ttx")
ttx.Scale(1/ttx.Integral())
ttx.SetLineColor(ROOT.kSpring)
hs.Add(ttx)

hs.Draw()
T = TText()
T.SetTextFont(42)
T.SetTextAlign(21)
T.DrawTextNDC(.5,.95,"nVert")
c1.BuildLegend(0.66, 0.85, 0.87, 0.65, "")
c1.SetTitle("nVert")

pdfName="PU_Test"
c1.SaveAs("plots/"+pdfName+".pdf")


'''
path="/lstore/cms/dbastos/Stop4Body/nTuples_v2018-08-24/"
nTuple1="ZJetsToNuNu_HT600to800.root"
nTuple2="Data_2017B_MetHT.root"

f1=ROOT.TFile(path+nTuple1,"READ")
f2=ROOT.TFile(path+nTuple2,"READ")

t1=f1.Get("bdttree")
t2=f2.Get("bdttree")
h1 = TH1F("h1","nVert",150,0,150)
h2 = TH1F("h2","nVert",150,0,150)
h1.SetLineColor(ROOT.kBlue)
h2.SetLineColor(ROOT.kRed)

t1.Draw("nVert>>h1","","goff")
t2.Draw("nVert>>h2","","goff")

b1=t1.GetBranch("nVert")
b2=t2.GetBranch("nVert")

nE1=b1.GetEntries()

maxVal=max(h1.GetMaximum(),h2.GetMaximum())
minVal=max(h1.GetMinimum(),h2.GetMinimum())
hs.Add(h1)
hs.Add(h2)

hs.Draw()

#for i in range(nE1):
#  h1.Fill(b1.GetEntry(i))
'''

