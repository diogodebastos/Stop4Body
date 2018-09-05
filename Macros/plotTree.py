import ROOT
from ROOT import TFile, TH1F, TCanvas, TTree, THStack

path="/lstore/cms/dbastos/Stop4Body/nTuples_v2018-08-24/"
nTuple1="ZJetsToNuNu_HT600to800.root"
nTuple2="Data_2017B_MetHT.root"

ROOT.gStyle.SetOptStat("00000000")

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

c1 = TCanvas("c1","nVert", 1200, 1350)

hs= THStack("hs","")
hs.Add(h1)
hs.Add(h2)

hs.Draw()

c1.BuildLegend(0.66, 0.85, 0.87, 0.75, "")

#for i in range(nE1):
#  h1.Fill(b1.GetEntry(i))
