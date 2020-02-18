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

def assure_path_exists(path):
  dir = os.path.dirname(path)
  if not os.path.exists(dir):
    os.makedirs(dir)

if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-i', '--inputDirectory', required=True, help='Name of the input directory')
  parser.add_argument('-o', '--outputDirectory', required=True, help='Base name of the output directory for each BDT')
  parser.add_argument( '--VR2', action='store_true', help='Whether to run for the second validation region, defined with 200 < Met < 280')
  parser.add_argument( '--VR3', action='store_true', help='Whether to run for the third validation region, defined with LepPt > 30')
  parser.add_argument('-d', '--dryRun', action='store_true', help='Do a dry run (i.e. do not actually run the potentially dangerous commands but print them to the screen)')

  args = parser.parse_args()

  inputDirectory = args.inputDirectory
  if not (os.path.exists(inputDirectory) and os.path.isdir(inputDirectory)):
    parser.error('The given input directory does not exist or is not a directory')

  if not args.dryRun:
    print "You did not enable dry run. You are on your own!"

  baseDirectory = os.path.realpath(os.getcwd())
  inputDirectory = os.path.realpath(inputDirectory)

### BDT Cuts for 2016
#  BDTs = [
#          {'name': '10', 'deltaM': 10, 'cut': 0.31, 'highDeltaM': False},
#          {'name': '10_alt', 'deltaM': 10, 'cut': 0.2, 'highDeltaM': False},
#          {'name': '20', 'deltaM': 20, 'cut': 0.39, 'highDeltaM': False},
#          {'name': '30', 'deltaM': 30, 'cut': 0.47, 'highDeltaM': False},
#          {'name': '40', 'deltaM': 40, 'cut': 0.48, 'highDeltaM': False},
#          {'name': '50', 'deltaM': 50, 'cut': 0.45, 'highDeltaM': False},
#          {'name': '60', 'deltaM': 60, 'cut': 0.50, 'highDeltaM': False},
#          {'name': '70', 'deltaM': 70, 'cut': 0.46, 'highDeltaM': True},
#          {'name': '80', 'deltaM': 80, 'cut': 0.44, 'highDeltaM': True},
#  ]

### BDT Cuts for 2017
  BDTs = [
          {'name': '10', 'deltaM': 10, 'cut': 0.31, 'highDeltaM': False,'doLoosenBDT': True},
          {'name': '20', 'deltaM': 20, 'cut': 0.36, 'highDeltaM': False,'doLoosenBDT': True},
          {'name': '30', 'deltaM': 30, 'cut': 0.46, 'highDeltaM': False,'doLoosenBDT': True},
          {'name': '40', 'deltaM': 40, 'cut': 0.46, 'highDeltaM': False,'doLoosenBDT': True},
          {'name': '50', 'deltaM': 50, 'cut': 0.47, 'highDeltaM': False,'doLoosenBDT': False},
          {'name': '60', 'deltaM': 60, 'cut': 0.51, 'highDeltaM': False,'doLoosenBDT': False},
          {'name': '70', 'deltaM': 70, 'cut': 0.43, 'highDeltaM': True,'doLoosenBDT': False},
          {'name': '80', 'deltaM': 80, 'cut': 0.46, 'highDeltaM': True,'doLoosenBDT': False},
  ]

  for bdt in BDTs:
    thisInputDirectory = inputDirectory + "_bdt" + str(bdt["deltaM"])
    outputDirectory = thisInputDirectory + "/" + args.outputDirectory + "/"
    print "Creating output directory for BDT_DM" + bdt['name']
    assure_path_exists(outputDirectory)
    outputDirectory = os.path.realpath(outputDirectory)

    thisInputDirectory = inputDirectory + "_bdt" + str(bdt["deltaM"])

    jobsDir = outputDirectory + "/Jobs/"
    assure_path_exists(jobsDir)
    jobsDir = os.path.realpath(jobsDir)

    job = jobsDir + "/theJob.sh"

    with open(job, 'w') as thisScript:
      logF = "/SystematicsLog"
      if args.VR2:
          logF += "_VR2"
      if args.VR3:
          logF += "_VR3"
      thisScript.write("#!/bin/bash\n\n")

      thisScript.write("alias cmsenv='eval `scramv1 runtime -sh`'\n\n")

      thisScript.write("export SCRAM_ARCH=slc6_amd64_gcc530\n")
      thisScript.write("export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch\n")
      thisScript.write("source $VO_CMS_SW_DIR/cmsset_default.sh\n")
      thisScript.write("export CMS_PATH=$VO_CMS_SW_DIR\n")
      thisScript.write("source /cvmfs/cms.cern.ch/crab3/crab.sh\n\n")

      thisScript.write("cd $CMSSW_BASE/src/\n")
      thisScript.write("eval `scramv1 runtime -sh`\n\n")

      thisScript.write("cd " + baseDirectory + "\n\n")

      thisScript.write(". setupJSONs.sh\n")
      thisScript.write(". setupPaths.sh\n\n")

      thisScript.write("getDDSystematics ")
      thisScript.write("--verbose ")
      thisScript.write("--json ${JSON_PATH}/plot2017_DM"+bdt['name']+"RP.json ")
      thisScript.write("--inDir " + thisInputDirectory + " ")
      thisScript.write("--outDir " + outputDirectory + " ")
      thisScript.write("--suffix bdt ")
      thisScript.write("--signalRegionCut ")
      thisScript.write(str(bdt['cut']) + " ")
      if args.VR2:
        thisScript.write("--doVR2 ")
      if args.VR3:
        thisScript.write("--doVR3 ")
      if bdt['doLoosenBDT']:
        thisScript.write("--doLoosenBDT ")
      if bdt['highDeltaM']:
        thisScript.write("--isHighDeltaM ")
      thisScript.write(" 1> " + outputDirectory + logF + ".log 2> " + outputDirectory + logF +".err")
      thisScript.write("\n\n")

      mode = os.fstat(thisScript.fileno()).st_mode
      mode |= 0o111
      os.fchmod(thisScript.fileno(), mode & 0o7777)

    os.chdir(jobsDir)
    cmd = "qsub -v CMSSW_BASE=$CMSSW_BASE " + job + " -e " + job + ".e$JOB_ID -o " + job + ".o$JOB_ID"
    print cmd
    if not args.dryRun:
      p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      out, err = p.communicate()
      print "Out:", out
      print "Err:", err

      if getNJobs() > 1700:
        print "Waiting for some jobs to complete..."
        while getNJobs() > 1000:
          time.sleep(5*60)
        print "Done waiting"
  os.chdir(baseDirectory)
