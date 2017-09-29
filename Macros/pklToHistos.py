import os,subprocess,ROOT
import pickle
import re
import time
import shutil

def findRange(values):
  vmin = None
  vmax = None
  dvmin = None
  vprev = None
  for v in sorted(list(values)):
    if vmin==None or v<vmin:
      vmin = v
    if vmax==None or v>vmax:
      vmax = v
    if vprev!=None:
      dv = abs(v-vprev)
      if dv>0 and ( dvmin==None or dv<dvmin ):
        dvmin = dv
    vprev = v
  return vmin,vmax,dvmin

def pklToHistos(inputPickle, outFile):
  labels = {
    '-1.000' : 'obs',
    '0.500' : 'exp',
    '0.840' : 'expM1',
    '0.160' : 'expP1',
    '0.975' : 'expM2',
    '0.025' : 'expP2'
  }

  d = pickle.load(file( inputPickle ) )

  xs = set()
  ys = set()
  ds = set()
  for x in d:
    xs.add(x)
    for y in d[x]:
      ys.add(y)
      ds.add(y-x)
      print x,y

  xmin,xmax,dxmin = findRange(xs)
  ymin,ymax,dymin = findRange(ys)
  dmin,dmax,ddmin = findRange(ds)
  print xmin,xmax,dxmin
  print dmin,dmax,ddmin

  xmin = xmin - dxmin/2.
  xmax = xmax + dxmin/2.
  nbx = int((xmax-xmin+dxmin/2.)/dxmin)
  ymin = ymin - dymin/2.
  ymax = ymax + dymin/2.
  nby = int((ymax-ymin+dymin/2.)/dymin)

  fout = ROOT.TFile( outFile,"recreate")
  histos = { }
  xaxis = None
  yaxis = None
  for v,l in labels.iteritems():
    histos[l] = ROOT.TH2F(l,l,nbx,xmin,xmax,nby,ymin,ymax)
    if xaxis==None:
      xaxis = histos[l].GetXaxis()
      yaxis = histos[l].GetYaxis()

  for x in d:
    ix = xaxis.FindBin(x)
    for y in d[x]:
      iy = yaxis.FindBin(y)
      for k in d[x][y]:
        if k in labels:
          histos[labels[k]].SetBinContent(ix,iy,d[x][y][k])

  fout.Write()
  print "Histogram read from: \n %s and written to: \n %s"%( inputPickle, outFile)
  return histos

if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-i', '--inputPickle', required=True, help='Name of the input pickle file')
  parser.add_argument('-o', '--outputFile', required=True, help='Name of the output file with the histograms')

  args = parser.parse_args()

  histos = pklToHistos(args.inputPickle, args.outputFile)
