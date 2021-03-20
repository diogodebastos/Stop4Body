#!/usr/bin/python

import os, sys

YEAR = os.getenv("YEAR")
path = "DataCards/"+YEAR
#path = "DataCards/"
dirs = os.listdir( path )

def myround(x, base=5):
    return int(base * round(float(x)/base))

for file in dirs:
  mStop = int(file[:3])
  mNeut = int(file[4:7])
  fixmStop = myround(mStop)
  fixmNeut = myround(mNeut)
  if mStop != fixmStop or mNeut != fixmNeut:
    print "mStop: " + str(mStop) + " => " + str(fixmStop)
    print "mNeut: " + str(mNeut) + " => " + str(fixmNeut) + "\n"
    newFile = path + str(fixmStop) + "_" + str(fixmNeut) + ".txt"
    os.rename(path+file, newFile)
