import os,subprocess,ROOT
import pickle
import re
import time

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

if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-i', '--inDirectory', required=True, help='Name of the input directory')
  parser.add_argument('-o', '--outDirectory', required=True, help='Name of the output directory')
  parser.add_argument('-w', '--weightDirectory', required=True, help='Name of the weights directory')
  parser.add_argument('-d', '--dryRun', action='store_true', help='Do a dry run (i.e. do not actually run the potentially dangerous commands but print them to the screen)')
  parser.add_argument(      '--onlyData', action='store_true', help='Whether to only run on data')

  args = parser.parse_args()

  print "Creating output directory"
  cmd = "mkdir -p " + args.outDirectory
  print "  ", cmd
  if not args.dryRun:
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()

  inputFiles = listDir(args.inDirectory + "/*.root")
  baseDirectory = os.path.realpath(os.getcwd())
  for file in inputFiles:
    baseName = os.path.basename(file)[:-5]
    baseDir = args.outDirectory + "/" + baseName
    if baseName == "puWeights":
      continue
    if args.onlyData and baseName[:4] != "Data":
      continue

    cmd = "mkdir -p " + baseDir
    print cmd
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()

    with open(baseDir + "/theJob.sh", 'w') as thisScript:
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

      thisScript.write("#. setupJSONs.sh\n")
      thisScript.write(". setupPaths.sh\n\n")

      thisScript.write("applyMVAFile ")
      thisScript.write("--inFile " + file + " ")
      thisScript.write("--outDir " + args.outDirectory + " ")
      thisScript.write("--weightDir " + args.weightDirectory + " ")
      thisScript.write("\n\n")

      mode = os.fstat(thisScript.fileno()).st_mode
      mode |= 0o111
      os.fchmod(thisScript.fileno(), mode & 0o7777)

    cwd = os.getcwd() # for now, we need to change the directory so that the out and err files are in the right place
    os.chdir(baseDir)

    jobInfo = {}
    #cmd = "qsub theJob.sh -e theJob.sh.e$JOB_ID -o theJob.sh.o$JOB_ID"
    cmd = "qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE theJob.sh -e theJob.sh.e$JOB_ID -o theJob.sh.o$JOB_ID"

    print cmd
    if not args.dryRun:
      p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      out, err = p.communicate()
      p = re.compile("Your job (\d+) .+")
      jobNumber = p.search(out).group(1)

      jobInfo["theJob.sh"] = jobNumber

    os.chdir(cwd)

    with open(baseDir + '/jobs.pickle', 'wb') as handle:
      pickle.dump(jobInfo, handle, protocol=pickle.HIGHEST_PROTOCOL)

#    if getNJobs() > 1700:
#      print "Waiting for some jobs to complete..."
#      while getNJobs() > 1000:
#        time.sleep(5*60)
#      print "Done waiting"
