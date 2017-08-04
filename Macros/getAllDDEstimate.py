import os,subprocess,ROOT
import pickle
import re
import time
import shutil

def assure_path_exists(path):
  dir = os.path.dirname(path)
  if not os.path.exists(dir):
    os.makedirs(dir)

if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-i', '--inputDirectory', required=True, help='Name of the input directory')
  parser.add_argument('-o', '--outputDirectory', required=True, help='Base name of the output directory for each BDT')
  #parser.add_argument('-j', '--jsonFile', required=True, help='The json file describing the samples to use')
  #parser.add_argument('-s', '--doSwap', action='store_true', help='Whether to process with the swapping of MET and LepPt variables')
  parser.add_argument( '--VRAlt', action='store_true', help='Whether to run for the alternative validation region, defined with 200 < Met < 280')
  parser.add_argument( '--isSwap', action='store_true', help='Set this flag if the samples being run on are the ones with the MET and LepPt variables swapped')
  parser.add_argument('-d', '--dryRun', action='store_true', help='Do a dry run (i.e. do not actually run the potentially dangerous commands but print them to the screen)')

  args = parser.parse_args()

  inputDirectory = args.inputDirectory
  if not (os.path.exists(inputDirectory) and os.path.isdir(inputDirectory)):
    parser.error('The given input directory does not exist or is not a directory')

  if args.isSwap and args.VRAlt:
    parser.error('It does not make sense to simulataneously define both conditions for the validation regions')

  if not args.dryRun:
    print "You did not enable dry run. You are on your own!"

  baseDirectory = os.path.realpath(os.getcwd())
  inputDirectory = os.path.realpath(inputDirectory)

  BDTs = [
          {'name': '10', 'deltaM': 10, 'cut': 0.31, 'highDeltaM': False},
          {'name': '10_alt', 'deltaM': 10, 'cut': 0.2, 'highDeltaM': False},
          {'name': '20', 'deltaM': 20, 'cut': 0.39, 'highDeltaM': False},
          {'name': '30', 'deltaM': 30, 'cut': 0.47, 'highDeltaM': False},
          {'name': '40', 'deltaM': 40, 'cut': 0.48, 'highDeltaM': False},
          {'name': '50', 'deltaM': 50, 'cut': 0.45, 'highDeltaM': False},
          {'name': '60', 'deltaM': 60, 'cut': 0.50, 'highDeltaM': False},
          {'name': '70', 'deltaM': 70, 'cut': 0.46, 'highDeltaM': True},
          {'name': '80', 'deltaM': 80, 'cut': 0.44, 'highDeltaM': True},
  ]

  for bdt in BDTs:
    outputDirectory = args.outputDirectory + "_bdt" + bdt['name']
    if args.VRAlt:
      outputDirectory = outputDirectory + "_VRAlt"
    if args.isSwap:
      outputDirectory = outputDirectory + "_Swap"
    outputDirectory = outputDirectory + "/"
    print "Creating output directory for '" + bdt['name'] + "'"
    assure_path_exists(outputDirectory)
    outputDirectory = os.path.realpath(outputDirectory)

    thisInputDirectory = inputDirectory + "_bdt" + str(bdt["deltaM"])

    thisScript = open(outputDirectory + "/theJob.sh", 'w')

    thisScript.write("#!/bin/bash\n\n")

    thisScript.write("alias cmsenv='eval `scramv1 runtime -sh`'\n\n")

    thisScript.write("cd /exper-sw/cmst3/cmssw/users/cbeiraod/\n")
    thisScript.write(". setup.sh\n\n")

    thisScript.write("cd $CMSSW_BASE/src/\n")
    thisScript.write("eval `scramv1 runtime -sh`\n\n")

    thisScript.write("cd " + baseDirectory + "\n\n")

    #thisScript.write(". setupJSONs.sh\n")
    thisScript.write(". setupPaths.sh\n\n")

    thisScript.write("getDDEstimate ")
    if args.isSwap:
      thisScript.write("--json ${JSON_PATH}/plot2016swap_lep.json ")
    else:
      thisScript.write("--json ${JSON_PATH}/plot2016_lep.json ")
    thisScript.write("--outDir " + outputDirectory + " ")
    thisScript.write("--inDir " + thisInputDirectory + " ")
    thisScript.write("--suffix bdt ")
    thisScript.write("--signalRegionCut ")
    if args.VRAlt:
      thisScript.write("0.2 --invertMet ")
    else:
      thisScript.write(str(bdt['cut']) + " ")
    if args.isSwap:
      thisScript.write("--isSwap ")
    if bdt['highDeltaM']:
      thisScript.write("--isHighDeltaM ")
    thisScript.write(" 1> " + outputDirectory + "/DDEstimateLog.log 2> " + outputDirectory + "/DDEstimateLog.err")
    thisScript.write("\n\n")

    #shutil.copy2("./variablesSR.json", outputDirectory + "/cutsJson.json")
    repldict = {'$(BDTCUT)':str(bdt['cut']), '$(highDeltaM)':' && (LepPt < 30)', '$(METCUT)':'Met > 280'}
    if args.isSwap:
      repldict['$(highDeltaM)'] = " && (LepPt < 30) && (HLT_Mu == 1)"
    if bdt['highDeltaM']:
      repldict['$(highDeltaM)'] = ""
      if args.isSwap:
        repldict['$(highDeltaM)'] = " && (LepPt < 280) && (HLT_Mu == 1)"
    if args.VRAlt:
      repldict['$(BDTCUT)'] = str(0.2)
      repldict['$(METCUT)'] = "Met > 200 && Met < 280"
    def replfunc(match):
      return repldict[match.group(0)]

    regex = re.compile('|'.join(re.escape(x) for x in repldict))
    with open("./variablesSR.json") as fin, open(outputDirectory + "/cutsJson.json",'w') as fout:
      for line in fin:
        fout.write(regex.sub(replfunc,line))

    thisScript.write("makePlots ")
    if args.isSwap:
      thisScript.write("--json ${JSON_PATH}/plot2016swap_lep.json ")
    else:
      thisScript.write("--json ${JSON_PATH}/plot2016_lep.json ")
    thisScript.write("--outDir " + outputDirectory + " ")
    thisScript.write("--inDir " + thisInputDirectory + " ")
    thisScript.write("--suffix bdt ")
    thisScript.write("--variables " + outputDirectory + "/cutsJson.json ")
    thisScript.write("--cuts " + outputDirectory + "/cutsJson.json ")
    if args.isSwap or args.VRAlt:
      thisScript.write("--unblind")
    thisScript.write(" 1> " + outputDirectory + "/makePlotsLog.log 2> " + outputDirectory + "/makePlotsLog.err")
    thisScript.write("\n\n")

    mode = os.fstat(thisScript.fileno()).st_mode
    mode |= 0o111
    os.fchmod(thisScript.fileno(), mode & 0o7777)

    job = outputDirectory + "/theJob.sh"
    cmd = "qsub " + job + " -e " + job + ".e$JOB_ID -o " + job + ".o$JOB_ID"
    print cmd
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()


















