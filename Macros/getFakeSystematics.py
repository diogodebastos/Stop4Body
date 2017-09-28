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
  parser.add_argument('-o', '--outputDirectory', required=True, help='Base name of the output directory for each DM')
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
          {'name': '10', 'deltaM': 10, 'cut': 0.31, 'highDeltaM': False},
          {'name': '20', 'deltaM': 20, 'cut': 0.39, 'highDeltaM': False},
          {'name': '30', 'deltaM': 30, 'cut': 0.47, 'highDeltaM': False},
          {'name': '40', 'deltaM': 40, 'cut': 0.48, 'highDeltaM': False},
          {'name': '50', 'deltaM': 50, 'cut': 0.45, 'highDeltaM': False},
          {'name': '60', 'deltaM': 60, 'cut': 0.50, 'highDeltaM': False},
          {'name': '70', 'deltaM': 70, 'cut': 0.46, 'highDeltaM': True},
          {'name': '80', 'deltaM': 80, 'cut': 0.44, 'highDeltaM': True},
  ]

  for bdt in BDTs:
    thisInputDirectory = inputDirectory + "_bdt" + str(bdt["deltaM"])

    closureOutputDirectory = args.outputDirectory + "/FakeClosure/DeltaM" + bdt['name'] + "/"
    print "Creating output directory for fake closure of '" + bdt['name'] + "'"
    assure_path_exists(closureOutputDirectory)
    closureOutputDirectory = os.path.realpath(closureOutputDirectory)

    with open(closureOutputDirectory + "/theJob.sh", 'w') as thisScript:
      thisScript.write("#!/bin/bash\n\n")

      thisScript.write("alias cmsenv='eval `scramv1 runtime -sh`'\n\n")

      thisScript.write("cd /exper-sw/cmst3/cmssw/users/cbeiraod/\n")
      thisScript.write(". setup.sh\n\n")

      thisScript.write("cd $CMSSW_BASE/src/\n")
      thisScript.write("eval `scramv1 runtime -sh`\n\n")

      thisScript.write("cd " + baseDirectory + "\n\n")

      thisScript.write("#. setupJSONs.sh\n")
      thisScript.write(". setupPaths.sh\n\n")

      thisScript.write("makePlots ")
      thisScript.write("--doSummary ")
      thisScript.write("--lumi 1 ")
      thisScript.write("--json ${JSON_PATH}/plot2016_closure.json ")
      thisScript.write("--outDir " + closureOutputDirectory + "/ ")
      thisScript.write("--inDir " + thisInputDirectory + "/ ")
      thisScript.write("--dofakeclosure ")
      thisScript.write("--variables fake_bdt10.json ")
      thisScript.write("--cuts fake_bdt" + bdt['name'] + ".json ")
      thisScript.write("--suffix bdt ")
      thisScript.write("\n\n")

      mode = os.fstat(thisScript.fileno()).st_mode
      mode |= 0o111
      os.fchmod(thisScript.fileno(), mode & 0o7777)

    job = closureOutputDirectory + "/theJob.sh"
    cmd = "qsub " + job + " -e " + job + ".e$JOB_ID -o " + job + ".o$JOB_ID"
    print cmd
    if not args.dryRun:
      p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      out, err = p.communicate()

    isrOutputDirectory = args.outputDirectory + "/FakeSystematics/DeltaM" + bdt['name'] + "/"
    print "Creating output directory for fake closure of '" + bdt['name'] + "'"
    assure_path_exists(isrOutputDirectory)
    isrOutputDirectory = os.path.realpath(isrOutputDirectory)

    with open(isrOutputDirectory + "/theJob.sh", 'w') as thisScript:
      thisScript.write("#!/bin/bash\n\n")

      thisScript.write("alias cmsenv='eval `scramv1 runtime -sh`'\n\n")

      thisScript.write("cd /exper-sw/cmst3/cmssw/users/cbeiraod/\n")
      thisScript.write(". setup.sh\n\n")

      thisScript.write("cd $CMSSW_BASE/src/\n")
      thisScript.write("eval `scramv1 runtime -sh`\n\n")

      thisScript.write("cd " + baseDirectory + "\n\n")

      thisScript.write("#. setupJSONs.sh\n")
      thisScript.write(". setupPaths.sh\n\n")

      thisScript.write("fakeSystematics ")
      thisScript.write("--json ${JSON_PATH}/plot2016_lep.json ")
      thisScript.write("--outDir " + isrOutputDirectory + "/ ")
      thisScript.write("--inDir " + thisInputDirectory + "/ ")
      thisScript.write("--suffix bdt ")
      thisScript.write("--variables fake_bdt10.json ")
      thisScript.write("--cuts fake_bdt" + bdt['name'] + ".json ")
      thisScript.write("\n\n")

      mode = os.fstat(thisScript.fileno()).st_mode
      mode |= 0o111
      os.fchmod(thisScript.fileno(), mode & 0o7777)

    job = isrOutputDirectory + "/theJob.sh"
    cmd = "qsub " + job + " -e " + job + ".e$JOB_ID -o " + job + ".o$JOB_ID"
    print cmd
    if not args.dryRun:
      p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      out, err = p.communicate()

    nuOutputDirectory = args.outputDirectory + "/FakeSystematics_NU/DeltaM" + bdt['name'] + "/"
    print "Creating output directory for fake closure of '" + bdt['name'] + "'"
    assure_path_exists(nuOutputDirectory)
    nuOutputDirectory = os.path.realpath(nuOutputDirectory)

    with open(nuOutputDirectory + "/theJob.sh", 'w') as thisScript:
      thisScript.write("#!/bin/bash\n\n")

      thisScript.write("alias cmsenv='eval `scramv1 runtime -sh`'\n\n")

      thisScript.write("cd /exper-sw/cmst3/cmssw/users/cbeiraod/\n")
      thisScript.write(". setup.sh\n\n")

      thisScript.write("cd $CMSSW_BASE/src/\n")
      thisScript.write("eval `scramv1 runtime -sh`\n\n")

      thisScript.write("cd " + baseDirectory + "\n\n")

      thisScript.write("#. setupJSONs.sh\n")
      thisScript.write(". setupPaths.sh\n\n")

      thisScript.write("fakeSystematics ")
      thisScript.write("--json ${JSON_PATH}/plot2016_lep.json ")
      thisScript.write("--outDir " + nuOutputDirectory + "/ ")
      thisScript.write("--inDir " + thisInputDirectory + "/ ")
      thisScript.write("--suffix bdt ")
      thisScript.write("--variables fake_bdt10.json ")
      thisScript.write("--cuts fake_bdt" + bdt['name'] + ".json ")
      thisScript.write("--doNonUniversality ")
      thisScript.write("\n\n")

      mode = os.fstat(thisScript.fileno()).st_mode
      mode |= 0o111
      os.fchmod(thisScript.fileno(), mode & 0o7777)

    job = nuOutputDirectory + "/theJob.sh"
    cmd = "qsub " + job + " -e " + job + ".e$JOB_ID -o " + job + ".o$JOB_ID"
    print cmd
    if not args.dryRun:
      p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      out, err = p.communicate()

      if getNJobs() > 1700:
        print "Waiting for some jobs to complete..."
        while getNJobs() > 1000:
          time.sleep(5*60)
        print "Done waiting"


















