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

  YEAR = os.getenv("YEAR")

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-i', '--inputDirectory', required=True, help='Name of the input directory')
  parser.add_argument('-o', '--outputDirectory', required=True, help='Base name of the output directory for DataCards')
#  parser.add_argument('-y', '--year', required=True, help='Base name of the output directory for DataCards')
  parser.add_argument('-d', '--dryRun', action='store_true', help='Do a dry run (i.e. do not actually run the potentially dangerous commands but print them to the screen)')

  args = parser.parse_args()

  inputDirectory = args.inputDirectory
  if not (os.path.exists(inputDirectory) and os.path.isdir(inputDirectory)):
    parser.error('The given input directory does not exist or is not a directory')

  if not args.dryRun:
  	print "You did not enable dry run. You are on your own!"
 
  baseDirectory = os.path.realpath(os.getcwd())
  inputDirectory = os.path.realpath(inputDirectory)
  BDTs=[]

  if YEAR == "2017":
    BDTs = [
		   {'name': '10', 'deltaM': 10, 'cut': 0.34, 
		   "WjSys": 0.183,"ttSys": 1.119, "FakeSys": 0.345, 'highDeltaM': False},
		   {'name': '20', 'deltaM': 20, 'cut': 0.42, 
		   "WjSys": 0.189,"ttSys": 0.404, "FakeSys": 0.065, 'highDeltaM': False},
		   {'name': '30', 'deltaM': 30, 'cut': 0.45, 
		   "WjSys": 0.167,"ttSys": 0.414, "FakeSys": 0.069, 'highDeltaM': False},
		   {'name': '40', 'deltaM': 10, 'cut': 0.34, 
		   "WjSys": 0.215,"ttSys": 0.304, "FakeSys": 0.073, 'highDeltaM': False},
		   {'name': '50', 'deltaM': 20, 'cut': 0.42, 
		   "WjSys": 0.152,"ttSys": 0.239, "FakeSys": 0.087, 'highDeltaM': False},
		   {'name': '60', 'deltaM': 30, 'cut': 0.45, 
		   "WjSys": 0.217,"ttSys": 0.179, "FakeSys": 0.078, 'highDeltaM': False},
		   {'name': '70', 'deltaM': 10, 'cut': 0.34, 
		   "WjSys": 0.215,"ttSys": 0.069, "FakeSys": 0.143, 'highDeltaM': True},
		   {'name': '80', 'deltaM': 20, 'cut': 0.42, 
		   "WjSys": 0.295,"ttSys": 0.076, "FakeSys": 0.092, 'highDeltaM': True},
           ]
  elif YEAR == "2018":
    BDTs = [
		   {'name': '10', 'deltaM': 10, 'cut': 0.39, 
		   "WjSys": 0.114,"ttSys": 0.513, "FakeSys": 0.324, 'highDeltaM': False},
		   {'name': '20', 'deltaM': 20, 'cut': 0.43, 
		   "WjSys": 0.140,"ttSys": 0.623, "FakeSys": 0.363, 'highDeltaM': False},
		   {'name': '30', 'deltaM': 30, 'cut': 0.48, 
		   "WjSys": 0.114,"ttSys": 0.387, "FakeSys": 0.299, 'highDeltaM': False},
		   {'name': '40', 'deltaM': 10, 'cut': 0.50, 
		   "WjSys": 0.216,"ttSys": 0.347, "FakeSys": 0.114, 'highDeltaM': False},
		   {'name': '50', 'deltaM': 20, 'cut': 0.49, 
		   "WjSys": 0.254,"ttSys": 0.294, "FakeSys": 0.189, 'highDeltaM': False},
		   {'name': '60', 'deltaM': 30, 'cut': 0.53, 
		   "WjSys": 0.147,"ttSys": 0.294, "FakeSys": 0.153, 'highDeltaM': False},
		   {'name': '70', 'deltaM': 10, 'cut': 0.49, 
		   "WjSys": 0.171,"ttSys": 0.243, "FakeSys": 0.314, 'highDeltaM': True},
		   {'name': '80', 'deltaM': 20, 'cut': 0.45, 
		   "WjSys": 0.204,"ttSys": 0.133, "FakeSys": 0.457, 'highDeltaM': True},
           ]

  for bdt in BDTs:
  	outputDirectory = "/lstore/cms/dbastos/REPOS/Stop4Body-nanoAOD/CMSSW_8_0_14/src/UserCode/Stop4Body-nanoAOD/Macros/"
    assure_path_exists(outputDirectory)
    outputDirectory = os.path.realpath(outputDirectory)
    thisInputDirectory = inputDirectory + "_bdt" + str(bdt["deltaM"])

    jobsDir = outputDirectory + "/Jobs/"
    assure_path_exists(jobsDir)
    jobsDir = os.path.realpath(jobsDir)

    job = jobsDir + "job_" + str(bdt["deltaM"]) + ".sh"

    with open(job, 'w') as thisScript:
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
      thisScript.write("genDataCardsForCLs ")
      thisScript.write("--inDir " + thisInputDirectory + " ")
      thisScript.write("--outDir " + outputDirectory + " ")
      # Do for only 1 SP
      #thisScript.write("--json ${JSON_PATH}/plot"+YEAR+"_DM"+bdt['name']+"RP.json ")
      thisScript.write("--json ${JSON_PATH}/allDM"+bdt['name']+".json ")
      thisScript.write("--suffix bdt ")
      thisScript.write("--bdtCut "  + str(bdt['cut']) + " ")
      thisScript.write("--year "    + YEAR + " ")
      thisScript.write("--WjSys "   + str(bdt['WjSys']) + " ")
      thisScript.write("--ttSys "   + str(bdt['ttSys']) + " ")
      thisScript.write("--fakeSys " + str(bdt['fakeSys']) + " ")
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