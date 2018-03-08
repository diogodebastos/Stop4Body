import os,subprocess,ROOT
import pickle
import re
import time
import shutil

import ctypes,array

hNames = {
  "expected" : "exp",
  "expPlus1" : "expP1",
  "expPlus2" : "expP2",
  "expMinus1" : "expM1",
  "expMinus2" : "expM2",
  "observed" : "obs"
}
xsrefName = "stop13TeV_NLONLL"
xsupName = "stop13TeV_NLONLL_Up"
xsdownName = "stop13TeV_NLONLL_Down"

def binning(xmin,xmax,dx):
  nb = int((xmax-xmin+(xmax-xmin)*0.001)/dx) + 1
  return (nb,xmin-dx/2.,xmax+dx/2.)

def getLimit(
    inputFile,
    histogramName,
    histogramSuffix,
    referenceCrossSection,
    crossSection=None,
    dmplot=False,
    debug=False,
    processAbs=False,
    mstopBinsTmp=(250,800,25),
    mstopRange=(250,800),
    dmstopIn=25,
    dmstopTmp=25,
    dmBinsTmp=(10,80,5),
    dmRange=(10,80),
    ddmIn=10,
    ddmTmp=5,
    mstopBinsOut=(250,800,25),
    mneutBinsOut=(170,790,5)
  ):
# Get the input histogram
  histogramIn = inputFile.Get(histogramName)
  if not histogramIn:
    return None
  histogramIn.UseCurrentStyle()
  xIn = histogramIn.GetXaxis()
  yIn = histogramIn.GetYaxis()

  ROOT.gROOT.cd()

  # Create the temporary histogram (always dm vs mstop)
  hnDmTmp = histogramName + histogramSuffix + "DmTmp"
  hDmTmp = ROOT.TH2F(
    hnDmTmp,
    hnDmTmp,
    mstopBinsTmp[0],
    mstopBinsTmp[1],
    mstopBinsTmp[2],
    dmBinsTmp[0],
    dmBinsTmp[1],
    dmBinsTmp[2]
  )
  xDmTmp = hDmTmp.GetXaxis()
  yDmTmp = hDmTmp.GetYaxis()

  # Copy the values from the input histogram to the temporary one
  for mstop in range(mstopRange[0],mstopRange[1]+dmstopIn,dmstopIn):
    ixIn = xIn.FindBin(mstop)
    ixDmTmp = xDmTmp.FindBin(mstop)
    for dm in range(dmRange[0],dmRange[1]+ddmIn,ddmIn):
      if dmplot:
          mneut = dm
      else:
          mneut = mstop - dm
      iyIn = yIn.FindBin(mneut)
      v = histogramIn.GetBinContent(ixIn,iyIn)
      iyDmTmp = yDmTmp.FindBin(dm)
      hDmTmp.SetBinContent(ixDmTmp,iyDmTmp,v)

  # If we are debugging, draw and save the tmp histogram
  if debug:
    print "hDmTmp",hnDmTmp
    cDmRel = ROOT.TCanvas("cDmRel"+histogramName,"cDmRel"+histogramName)
    hDmTmp.SetMinimum(0.)
    hDmTmp.SetMaximum(2.)
    hDmTmp.DrawCopy("zcol")
    cDmRel.Update()
    cDmRel.SaveAs(cDmRel.GetName()+".png")

  # If we set the option to use the absolute value of the cross section instead of the signal strength
  if processAbs:
    for mstop in range(mstopRange[0],mstopRange[1]+dmstopTmp,dmstopTmp):
      ixDmTmp = xDmTmp.FindBin(mstop)
      for dm in range(dmRange[0],dmRange[1]+ddmTmp,ddmTmp):
        iyDmTmp = yDmTmp.FindBin(dm)
        v = hDmTmp.GetBinContent(ixDmTmp,iyDmTmp)
        v *= referenceCrossSection[mstop]
        hDmTmp.SetBinContent(ixDmTmp,iyDmTmp,v)
    # Once again, we draw and save the histogram if debugging
    if debug:
      print "Abs",hnDmTmp
      cDmAbs = ROOT.TCanvas("cDmAbs"+histogramName,"cDmAbs"+histogramName)
      hDmTmp.SetMinimum(0.)
      hDmTmp.DrawCopy("zcol")
      cDmAbs.Update()
      cDmAbs.SaveAs(cDmAbs.GetName()+".png")

  # Interpolate the histogram
  hinter = ROOT.interpolate(hDmTmp,"EW")
  hninter = histogramName + histogramSuffix + "Inter"
  hinter.SetNameTitle(hninter,hninter)

  # If debugging, save the interpolated plot
  if debug:
    cinter = ROOT.TCanvas("cinter"+histogramName,"cinter"+histogramName)
    hinter.SetMinimum(0.)
    if not doAbs:
      hinter.SetMaximum(2.)
    print "hinter",hninter
    hinter.Draw("zcol")
    cinter.Update()
    cinter.SaveAs(cinter.GetName()+".png")

  # Smooth the histogram
  hsmooth = ROOT.doSmooth(hinter,1)
  hnsmooth = histogramName + histogramSuffix + "Smooth"
  hsmooth.SetNameTitle(hnsmooth,hnsmooth)

  # If debugging, save the smoothed plot
  if debug:
    csmooth = ROOT.TCanvas("csmooth"+histogramName,"csmooth"+histogramName)
    #if crossSection==None:
    #  hsmooth.SetMaximum(10.)
    #hsmooth.SetMaximum(2.)
    hsmooth.DrawCopy("zcol")
    csmooth.Update()
    csmooth.SaveAs(csmooth.GetName()+".png")

  # If not processing abs, rescale the smoothed histogram to the limit on the cross section
  if not processAbs:
    for mstop in range(mstopRange[0],mstopRange[1]+dmstopTmp,dmstopTmp):
      ixDmTmp = xDmTmp.FindBin(mstop)
      for dm in range(dmRange[0],dmRange[1]+ddmTmp,ddmTmp):
        iyDmTmp = yDmTmp.FindBin(dm)
        v = hsmooth.GetBinContent(ixDmTmp,iyDmTmp)
        v *= referenceCrossSection[mstop]
        hsmooth.SetBinContent(ixDmTmp,iyDmTmp,v)
    # Once again, we draw and save the histogram if debugging
    if debug:
      cDmAbs = ROOT.TCanvas("cDmAbs"+histogramName,"cDmAbs"+histogramName)
      hsmooth.SetMinimum(0.)
      #hsmooth.SetMaximum(2.)
      hsmooth.DrawCopy("zcol")
      cDmAbs.Update()
      cDmAbs.SaveAs(cDmAbs.GetName()+".png")

  # Create the final output histograms, i.e. convert into mstop mneut if necessary
  hnout = histogramName + histogramSuffix + "Out"
  hout = ROOT.TH2F(
    hnout,
    hnout,
    mstopBinsOut[0],
    mstopBinsOut[1],
    mstopBinsOut[2],
    mneutBinsOut[0],
    mneutBinsOut[1],
    mneutBinsOut[2]
  )
  xOut = hout.GetXaxis()
  yOut = hout.GetYaxis()
  for mstop in range(mstopRange[0],mstopRange[1]+dmstopTmp,dmstopTmp):
    ixDmTmp = xDmTmp.FindBin(mstop)
    ixOut = xOut.FindBin(mstop)
    for dm in range(dmRange[0],dmRange[1]+ddmTmp,ddmTmp):
      if dmplot:
          mneut = dm
      else:
          mneut = mstop - dm
      iyDmTmp = yDmTmp.FindBin(dm)
      v = hsmooth.GetBinContent(ixDmTmp,iyDmTmp)
      iyOut = yOut.FindBin(mneut)
      hout.SetBinContent(ixOut,iyOut,v)
  if debug:
    cout = ROOT.TCanvas("cout"+histogramName,"cout"+histogramName)
    hout.Draw("zcol")
    cout.Update()
    cout.SaveAs(cout.GetName()+".png")

  # Scale the limits back to a signal strength using the chosen cross section (important for the variations on the observed limit)
  csmoothMin = None
  csmoothMax = None
  for mstop in range(mstopRange[0],mstopRange[1]+dmstopTmp,dmstopTmp):
    ixDmTmp = xDmTmp.FindBin(mstop)
    for dm in range(dmRange[0],dmRange[1]+ddmTmp,ddmTmp):
      iyDmTmp = yDmTmp.FindBin(dm)
      v = hsmooth.GetBinContent(ixDmTmp,iyDmTmp)
      if crossSection:
          v /= crossSection[mstop]
      else:
          v /= referenceCrossSection[mstop]
      hsmooth.SetBinContent(ixDmTmp,iyDmTmp,v)
      if csmoothMin==None or v<csmoothMin:
        csmoothMin = v
      if csmoothMax==None or v>csmoothMax:
        csmoothMax = v

  # Get the contour
  ccont = ROOT.TCanvas("ccont","ccont")
  contlist = [ ]
  for v in [0.5,1.0,1.5]:
    if v>=csmoothMin and v<=csmoothMax:
      contlist.append(v)
  if len(contlist)==0:
    contlist = [ 1.0 ]
  c_contlist = ((ctypes.c_double)*(len(contlist)))(*contlist)
  print len(contlist),c_contlist
  hsmooth.SetContour(len(contlist),c_contlist)
  hsmooth.Draw("contzlist")
  ccont.Update()
  # Retrieve contour we want and delete temporary canvas
  idx = contlist.index(1.0)
  contours_obs = ROOT.gROOT.GetListOfSpecials().FindObject("contours")
  graph_list = contours_obs.At(idx)
  del ccont

  # If we are debugging, draw the contour and save it
  if debug:
    print "Sample points on graphs"
    for g in graph_list:
      x = ROOT.Double(0.)
      y = ROOT.Double(0.)
      g.GetPoint(g.GetN()/2,x,y)
      print g.GetN()/2,x,y
    csmooth.cd()
    for g in graph_list:
      g.SetLineWidth(3)
      g.Draw("csame")
    csmooth.Update()
    csmooth.SaveAs(csmooth.GetName()+".png")

  # Transform the graph into its final form (mstop vs mneut if not doing the dm plot)
  graphs = [ ]
  for g in graph_list:
    graph = ROOT.TGraph()
    xp = ROOT.Double(0.)
    yp = ROOT.Double(0.)
    for ip in range(g.GetN()):
      g.GetPoint(ip,xp,yp)
      if dmplot:
          graph.SetPoint(ip,xp,yp)
      else:
          graph.SetPoint(ip,xp,xp-yp)
    graphs.append(graph.Clone())

  # If debugging, draw and save the transformed graphs
  if debug:
    cout.cd()
    for g in graphs:
      g.SetLineWidth(3)
      g.Draw("csame")
    cout.Update()
    cout.SaveAs(cout.GetName()+".png")

  # Clear up the memory
  del contours_obs
  for g in graph_list:
    del g

  if debug:
    raw_input("Finished producing debug histograms and plots for " + histogramName + histogramSuffix + ". Press enter to continue.")

  return ( hout.Clone(), graphs )

if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-i', '--inputFile', required=True, help='Name of the input file with the histograms to be smoothed')
  parser.add_argument('-o', '--outputFile', required=True, help='Name of the output file with the histograms')
  parser.add_argument('-d', '--debug', action='store_true', help='Enable debug mode')
  parser.add_argument('--dmplot', action='store_true', help='boh')
  parser.add_argument('--processAbs', action='store_true', help='Make the intermediate processing plots with the limit on the absolute value of the cross section and not with the limit on the signal strength')

  args = parser.parse_args()

  ROOT.gROOT.ProcessLine(".L setupColorPalette.C")
  ROOT.setupColorPalette()

  ROOT.gStyle.SetOptStat(0)
  ROOT.gStyle.SetOptTitle(0)

  ROOT.gROOT.ProcessLine(".L interpolate.h+")
  ROOT.gROOT.ProcessLine(".L LimitSmoothing.C+")

  execfile("xsecSMS.py")
  exec("xsrefs = "+xsrefName)
  exec("xsups = "+xsupName)
  exec("xsdowns = "+xsdownName)

  mstopRange = ( 250, 800 )
  dmRange = ( 10, 80 )
  dmstopIn = 25
  dmstopTmp = 25
  ddmIn = 10
  ddmTmp = 5
  if args.dmplot:
    ddmTmp = 10

  results = { }

  mstopBinsIn = binning(mstopRange[0],mstopRange[1],dmstopIn)
  mstopBinsTmp = binning(mstopRange[0],mstopRange[1],dmstopTmp)
  mstopBinsOut = binning(mstopRange[0],mstopRange[1],dmstopTmp)
  dmBinsIn  = binning(dmRange[0],dmRange[1],ddmIn )
  dmBinsTmp = binning(dmRange[0],dmRange[1],ddmTmp)
  if args.dmplot:
    mneutBinsOut = dmBinsTmp[:] #binning(mstopRange[0]-dmRange[1],mstopRange[1]-dmRange[0],ddmTmp)
  else:
    mneutBinsOut = binning(mstopRange[0]-dmRange[1],mstopRange[1]-dmRange[0],ddmTmp)

  commonOptions = {
    "dmplot"       : args.dmplot,
    "debug"        : args.debug,
    "processAbs"   : args.processAbs,
    "mstopBinsTmp" : mstopBinsTmp,
    "mstopRange"   : mstopRange,
    "dmstopIn"     : dmstopIn,
    "dmstopTmp"    : dmstopTmp,
    "dmBinsTmp"    : dmBinsTmp,
    "dmRange"      : dmRange,
    "ddmIn"        : ddmIn,
    "ddmTmp"       : ddmTmp,
    "mstopBinsOut" : mstopBinsOut,
    "mneutBinsOut" : mneutBinsOut,
  }

  fin = ROOT.TFile(args.inputFile)
  for t in hNames:
    if t=="observed":
      results[t] = getLimit(fin,hNames[t],"",xsrefs, **commonOptions)
      results[t+"Up"] = getLimit(fin,hNames[t],"Up",xsrefs,crossSection=xsups, **commonOptions)
      results[t+"Down"] = getLimit(fin,hNames[t],"Down",xsrefs,crossSection=xsdowns, **commonOptions)
    else:
      results[t] = getLimit(fin,hNames[t],"",xsrefs, **commonOptions)

  cres = ROOT.TCanvas("smoothed_limits","smoothed_limits",800,700)
  cres.SetLeftMargin(0.15)
  cres.SetRightMargin(0.20)
  if results["observed"] == None:
    hobs = results["expected"][0]
  else:
    hobs = results["observed"][0]
  hobs.GetXaxis().SetTitle("m(#tilde{t}) [GeV]")
  hobs.GetYaxis().SetTitle("m(#tilde{#chi}^{0}) [GeV]")
  #if results["observed"] == None:
  #  hobs.GetZaxis().SetTitle("expected 95% CL upper limit on cross section [pb]")
  #else:
  #  hobs.GetZaxis().SetTitle("95% CL upper limit on cross section [pb]")
  hobs.GetXaxis().SetTitleOffset(0.80)
  hobs.GetYaxis().SetTitleOffset(1.30)
  hobs.GetZaxis().SetTitleOffset(1.40)
  hobs.GetXaxis().SetTitleSize(0.05)
  hobs.GetYaxis().SetTitleSize(0.05)
  hobs.GetZaxis().SetTitleSize(0.05)
  hobs.GetXaxis().SetTitleFont(42)
  hobs.GetYaxis().SetTitleFont(42)
  hobs.GetZaxis().SetTitleFont(42)
  hobs.SetMinimum(0.1)
  hobs.SetMaximum(100.)
  hobs.Draw("zcol")
  ROOT.gPad.SetLogz(1)

  for t,r in results.iteritems():
    if r==None:
      continue
    for g in r[1]:
      if t.startswith("observed"):
        g.SetLineColor(2)
      else:
        g.SetLineColor(1)
      if t=="observed":
        g.SetLineWidth(3)
      elif t=="expected":
        g.SetLineWidth(3)
        g.SetLineStyle(2)
      else:
        g.SetLineWidth(2)
        g.SetLineStyle(3)
      g.Draw("csame")

  header = ROOT.TLatex()
  header.SetTextFont(42)
  header.SetTextAlign(11)
  header.SetTextSize(0.04)
  header.SetNDC(1)
  header.DrawLatex(0.16,0.925,'CMS preliminary  L=35.9fb^{-1}  #sqrt{s} = 13TeV')

  desc = ROOT.TLatex()
  desc.SetTextFont(42)
  desc.SetTextAlign(11)
  desc.SetTextSize(0.03)
  desc.SetNDC(1)

  ROOT.gPad.Update()

  cres.SaveAs("smoothed_limits.png")
  cres.SaveAs("smoothed_limits.pdf")
  cres.SaveAs("smoothed_limits.root")

  print results.keys()
  outNameDict = {
    "observed" : "OBSOut",
    "observedDown" : "OBSDownOut",
    "observedUp" : "OBSUpOut",
    "expected" : "EXPOut",
    "expMinus1" : "M1SOut",
    "expMinus2" : "M2SOut",
    "expPlus1" : "P1SOut",
    "expPlus2" : "P2SOut"
  }

  flim = ROOT.TFile("%s"%args.outputFile,"recreate")
  for k,n in outNameDict.iteritems():
    if results[k] == None:
      continue
    # write central observed and expected histograms
    if k=="expected" or k=="observed":
      h = results[k][0].Clone(n)
      h.Write()
    graphs = results[k][1]
    # check that there is only 1 graph
    #assert len(graphs)==1, graphs
    # write graph
    g = graphs[0].Clone("g"+n+"0")
    g.Write()
  flim.Close()

  #raw_input("Enter")













