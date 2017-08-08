import os,subprocess,ROOT
import pickle
import re
import time

if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-i', '--inDirectory', required=True, help='Name of the input directory')
  parser.add_argument('-o', '--outDirectory', required=True, help='Name of the output directory')
  parser.add_argument('-l', '--limitEvents', help='Whether to limit the number of events in the skimmed files')
  parser.add_argument('-d', '--dryRun', action='store_true', help='Do a dry run (i.e. do not actually run the potentially dangerous commands but print them to the screen)')

  args = parser.parse_args()

  if not args.dryRun:
    print "You did not enable dry run. You are on your own!"

  jsonFiles = [#"MC2Process.json",
               "Wjets.json",
               "TTbar.json",
               "TTbar_LO.json",
               "TTLep.json",
               "TT_pow.json",
               "otherMC2.json",
               "stop250.json",
               "stop275.json",
               "stop300.json",
               "stop325.json",
               "stop350.json",
               "stop375.json",
               "stop400.json",
               "stop425.json",
               "stop450.json",
               "stop475.json",
               "stop500.json",
               "stop525.json",
               "stop550.json",
               "stop575.json",
               "stop600.json",
               "stop625.json",
               "stop650.json",
               "stop675.json",
               "stop700.json",
               "stop725.json",
               "stop750.json",
               "stop775.json",
               "stop800.json"]


  print "Creating output directories"
  cmd = "mkdir -p " + args.outDirectory
  print "  ", cmd
  if not args.dryRun:
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()


  for json in jsonFiles:
    print "Processing the samples"
    cmd = "skimmer"
    cmd += " --json JSON/" + json
    cmd += " --inDir " + args.inDirectory
    cmd += " --outDir " + args.outDirectory
    if args.limitEvents is not None:
      cmd += " --limit " + args.limitEvents
    print "  ", cmd
    if not args.dryRun:
      p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      out, err = p.communicate()


















