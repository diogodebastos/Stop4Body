import os,subprocess,ROOT
import pickle
import re
import time
import shutil

def getNJobs():
  cmd = "qstat | wc -l"
  p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = p.communicate()

  return int(out)

def listDir(path):
  cmd = "ls " + path
  p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = p.communicate()
  return out.split()

def assure_path_exists(path):
  dir = os.path.dirname(path)
  if not os.path.exists(dir):
    os.makedirs(dir)

if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-i', '--inputDirectory', required=True, help='Name of the input directory')
#  parser.add_argument('-o', '--outputDirectory', required=True, help='Base name of the output directory for each BDT')
  #parser.add_argument('-j', '--jsonFile', required=True, help='The json file describing the samples to use')
  #parser.add_argument('-s', '--doSwap', action='store_true', help='Whether to process with the swapping of MET and LepPt variables')
  parser.add_argument('-d', '--dryRun', action='store_true', help='Do a dry run (i.e. do not actually run the potentially dangerous commands but print them to the screen)')

  args = parser.parse_args()

  inputDirectory = args.inputDirectory
  if not (os.path.exists(inputDirectory) and os.path.isdir(inputDirectory)):
    parser.error('The given input directory does not exist or is not a directory')

  if not args.dryRun:
    print "You did not enable dry run. You are on your own!"

  baseDirectory = os.path.realpath(os.getcwd())
  inputDirectory = os.path.realpath(inputDirectory)

  BDTs = [
#          {'name': '10', 'deltaM': 10, 'cut': 0.31, 'highDeltaM': False},
#          {'name': '20', 'deltaM': 20, 'cut': 0.39, 'highDeltaM': False},
#          {'name': '30', 'deltaM': 30, 'cut': 0.47, 'highDeltaM': False},
#          {'name': '40', 'deltaM': 40, 'cut': 0.48, 'highDeltaM': False},
#          {'name': '50', 'deltaM': 50, 'cut': 0.45, 'highDeltaM': False},
#          {'name': '60', 'deltaM': 60, 'cut': 0.50, 'highDeltaM': False},
#          {'name': '70', 'deltaM': 70, 'cut': 0.46, 'highDeltaM': True},
          {'name': '80', 'deltaM': 80, 'cut': 0.44, 'highDeltaM': True},
  ]

  for bdt in BDTs:
    thisInputDirectory = inputDirectory + "_bdt" + str(bdt["deltaM"])
    #outputDirectory = args.outputDirectory + "_bdt" + bdt['name'] + "/"
    #outputDirectory = thisInputDirectory + "/" + args.outputDirectory + "/"
    outputDirectory = thisInputDirectory + "/BdtCut/"
    print "Creating output directory for '" + bdt['name'] + "'"
    assure_path_exists(outputDirectory)
    outputDirectory = os.path.realpath(outputDirectory)

    dataCardsDir = outputDirectory + "/DataCards/"
    assure_path_exists(dataCardsDir)
    dataCardsDir = os.path.realpath(dataCardsDir)

    jobDir = dataCardsDir + "/Jobs/"
    assure_path_exists(jobDir)
    jobDir = os.path.realpath(jobDir)

    dataCards = os.listdir(dataCardsDir)

    cwd = os.getcwd()

    for dataCard in dataCards:
        bin = dataCard[0:9]

        job = jobDir + "/Job_" + bin + ".sh"
        with open(job, 'w') as thisScript:
            thisScript.write("#!/bin/bash\n\n")

            thisScript.write("alias cmsenv='eval `scramv1 runtime -sh`'\n\n")

            thisScript.write("cd /exper-sw/cmst3/cmssw/users/cbeiraod/\n")
            thisScript.write(". setup.sh\n\n")

            thisScript.write("cd $CMSSW_BASE/src/\n")
            thisScript.write("cd /exper-sw/cmst3/cmssw/users/cbeiraod/Stop4Body/CMSSW_8_1_0/src/\n")
            thisScript.write("eval `scramv1 runtime -sh`\n\n")

            thisScript.write("cd " + dataCardsDir + "\n\n")

            thisScript.write("combine -M Asymptotic ")
            thisScript.write(dataCard + " -n " + bin + " > " + dataCard[:-4] + "_asym.txt")
            thisScript.write("\n\n")

            mode = os.fstat(thisScript.fileno()).st_mode
            mode |= 0o111
            os.fchmod(thisScript.fileno(), mode & 0o7777)

        os.chdir(jobDir)
        cmd = "qsub -v CMSSW_BASE=$CMSSW_BASE " + job + " -e " + job + ".e$JOB_ID -o " + job + ".o$JOB_ID"
        print cmd
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()

    os.chdir(cwd)
