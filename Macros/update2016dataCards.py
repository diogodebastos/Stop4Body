#!/usr/bin/python

import os, sys

YEAR = os.getenv("YEAR")
#pathInp = "/Users/ketchum/tmp/DataCards/2016"
#pathOut = "/Users/ketchum/tmp/DataCards/2016_new"
pathInp = "DataCards/2016"
pathOut = "DataCards/2016_new"
dirs = os.listdir(pathInp)

def updateLine(line, sys, out):
  updated = False 
  if line.startswith(sys):
    if sys == "Lum":
      line = "Lum16     lnN 1.01 - - - 1.01 1.01 1.01 1.01"
    else:
      line = line.replace(sys, sys+"16")
    out.write(line)
    if sys == "Lum": out.write("\nLum161718 lnN 1.006 - - - 1.006 1.006 1.006 1.006\n")
    updated = True
  return updated

for file in dirs:
  with open(pathInp+"/"+file, 'r') as inp, open(pathOut+"/"+file, 'w') as out:
    for line in inp:
      if line.strip():

        Sstline   = updateLine(line,"Sst",out)
        FastSline = updateLine(line,"FastS",out)
        VVstline  = updateLine(line,"VVst",out)
        STstline  = updateLine(line,"STst",out)
        DYstline  = updateLine(line,"DYst",out)
        TTXstline = updateLine(line,"TTXst",out)
        Lumline   = updateLine(line,"Lum",out)
        Trgline   = updateLine(line,"Trg",out)
        JERline   = updateLine(line,"JER",out)
        IDline    = updateLine(line,"BTAG1",out)
        IDline    = updateLine(line,"BTAG2",out)
        IDline    = updateLine(line,"BTAG4",out)
        IDline    = updateLine(line,"BTAG5",out)
        IDline    = updateLine(line,"BTAG7",out)
        IDline    = updateLine(line,"BTAG8",out)
        IDline    = updateLine(line,"ID",out)
        ISOline   = updateLine(line,"ISO",out)

        if not (Sstline or FastSline or VVstline or STstline or DYstline or TTXstline or Lumline or Trgline or JERline or IDline or ISOline): out.write(line)
  
  inp.close()
  out.close()

