import os,subprocess,ROOT
import pickle
import re
import time

def getNJobs():
  cmd = "qstat | wc -l"
  p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = p.communicate()

  return int(out)

if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-i', '--inDirectory', required=True, help='Name of the input directory')
  parser.add_argument(      '--overloadTestOut', default="", help='Use to define an alternative output directory for the test events')
  parser.add_argument(      '--overloadTrainOut', default="", help='Use to define an alternative output directory for the train events')
  parser.add_argument(      '--previousTestEvents', help='Use to define where the test nTuples from a previous split are located, to be used for this splitting as well')
  parser.add_argument(      '--previousTrainEvents', help='Use to define where the train nTuples from a previous split are located, to be used for this splitting as well')
  parser.add_argument('-d', '--dryRun', action='store_true', help='Do a dry run (i.e. do not actually run the potentially dangerous commands but print them to the screen)')
  parser.add_argument(      '--submit', action='store_true', help='Whether to submit individual jobs to the batch system to parallelize the processing')

  args = parser.parse_args()

  if len([x for x in (args.previousTestEvents,args.previousTrainEvents) if x is not None]) == 1:
    parser.error('--previousTestEvents and --previousTrainEvents must be given together')

  if not args.dryRun:
    print "You did not enable dry run. You are on your own!"

  jsonFiles = [#"MC2Process.json",
               "Wjets.json",
               #"TTbar.json",
               #"TTbar_LO.json",
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
  #jsonFiles = ["otherMC2.json"] # AKA: ZInv.json
  #if args.doSwap:
  #  jsonFiles.append("DataSingleLepton.json")
  #else:
  #  jsonFiles.append("Data.json")


  testOut = args.inDirectory + "_test"
  if (args.overloadTestOut is not None) and (len(args.overloadTestOut) != 0):
    testOut = args.overloadTestOut
  trainOut = args.inDirectory + "_train"
  if (args.overloadTrainOut is not None) and (len(args.overloadTrainOut) != 0):
    trainOut = args.overloadTrainOut

  print "Creating output directories"
  cmd = "mkdir -p " + testOut
  print "  ", cmd
  if not args.dryRun:
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
  cmd = "mkdir -p " + trainOut
  print "  ", cmd
  if not args.dryRun:
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()

  if not args.submit:
    for json in jsonFiles:
      print "Splitting the samples"
      cmd = "splitTrainingTest"
      cmd += " --json JSON/" + json
      cmd += " --inDir " + args.inDirectory
      cmd += " --testOutDir " + testOut
      cmd += " --trainOutDir " + trainOut
      if len([x for x in (args.previousTestEvents,args.previousTrainEvents) if x is not None]) == 2:
        cmd += " --trainTreeDir " + args.previousTrainEvents
        cmd += " --testTreeDir " + args.previousTestEvents
      print "  ", cmd
      if not args.dryRun:
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()
  else:
    import json
    processedFiles = []
    baseDirectory = os.path.realpath(os.getcwd())
    for file in jsonFiles:
      jsonFileName = "JSON/" + file
      with open(jsonFileName) as data_file:
        data = json.load(data_file)
        for i in range(len(data["lines"])):
          for j in range(len(data["lines"][i]["files"])):
            sample = data["lines"][i]["files"][j]["tag"]
            cmd = "mkdir -p " + trainOut + "/" + sample
            print cmd
            p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
            out, err = p.communicate()

            with open(trainOut + "/" + sample + "/theJob.sh", 'w') as thisScript:
              thisScript.write("#!/bin/bash\n\n")

              thisScript.write("alias cmsenv='eval `scramv1 runtime -sh`'\n\n")

              thisScript.write("cd /exper-sw/cmst3/cmssw/users/cbeiraod/\n")
              thisScript.write(". setup.sh\n\n")

              thisScript.write("cd $CMSSW_BASE/src/\n")
              thisScript.write("eval `scramv1 runtime -sh`\n\n")

              thisScript.write("cd " + baseDirectory + "\n\n")

              thisScript.write("#. setupJSONs.sh\n")
              thisScript.write(". setupPaths.sh\n\n")

              thisScript.write("splitFileTrainingTest ")
              thisScript.write("--inFile " + args.inDirectory + "/" + sample + ".root ")
              thisScript.write("--testOutFile " + testOut + "/" + sample + ".root ")
              thisScript.write("--trainOutFile " + trainOut + "/" + sample + ".root ")
              if len([x for x in (args.previousTestEvents,args.previousTrainEvents) if x is not None]) == 2:
                thisScript.write("--testTreeFile " + args.previousTestEvents + "/" + sample + ".root ")
                thisScript.write("--trainTreeFile " + args.previousTrainEvents + "/" + sample + ".root ")
              thisScript.write("\n\n")

              mode = os.fstat(thisScript.fileno()).st_mode
              mode |= 0o111
              os.fchmod(thisScript.fileno(), mode & 0o7777)

            cwd = os.getcwd() # for now, we need to change the directory so that the out and err files are in the right place
            os.chdir(trainOut + "/" + sample)

            jobInfo = {}
            cmd = "qsub theJob.sh -e theJob.sh.e$JOB_ID -o theJob.sh.o$JOB_ID"
            print cmd
            if not args.dryRun:
              p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
              out, err = p.communicate()
              p = re.compile("Your job (\d+) .+")
              jobNumber = p.search(out).group(1)

              jobInfo["theJob.sh"] = jobNumber

            os.chdir(cwd)

            with open(trainOut + "/" + sample + '/jobs.pickle', 'wb') as handle:
              pickle.dump(jobInfo, handle, protocol=pickle.HIGHEST_PROTOCOL)

            if getNJobs() > 1700:
              print "Waiting for some jobs to complete..."
              while getNJobs() > 1000:
                time.sleep(5*60)
              print "Done waiting"


















