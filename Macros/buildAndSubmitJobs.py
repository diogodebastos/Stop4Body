import os,subprocess,ROOT
import pickle
import re

if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-o', '--outDirectory', required=True, help='Name of the output directory')
  parser.add_argument('-j', '--jsonTemplate', default="./Templates/jsonTemplate.json", help='Template for the json file for each job')
  parser.add_argument('-t', '--jobTemplate', default="./Templates/Step1_JobTemplate.sh", help='Template for the script for each job')
  parser.add_argument('-s', '--doSwap', action='store_true', help='Whether to process with the swapping of MET and LepPt variables') #TODO: Finish implementing
  parser.add_argument('-d', '--dryRun', action='store_true', help='Do a dry run (i.e. do not actually run the commands to remove the corrupt files from the output and the correctly copied files from the input)')

  args = parser.parse_args()

  if not args.dryRun:
    print "You did not enable dry run. You are on your own!"

  jsonFiles = ["MC2Process.json", "Data.json"]

  cmd = "buildJobs --template " + args.jobTemplate + " --jsonTemplate " + args.jsonTemplate + " --outDir " + args.outDirectory

  for json in jsonFile:
    thisCMD = cmd + " --samples JSON/" + json
    if args.dryRun:
      print "Going to run command:", thisCMD
    if not args.dryRun:
      p = subprocess.Popen(thisCMD, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      out, err = p.communicate()

  cmd = "ls -d " + args.outDirectory + "/*/"
  p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = p.communicate()
  samples = out.split()

  for sample in samples:
    cmd = "ls " + args.outDirectory + "/*.sh"
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    jobs = out.split()

    jobInfo = {}
    for job in jobs:
      cmd = "qsub " + args.outDirectory + "/" + job
      if args.dryRun:
        print "Going to run command:", cmd
      if not args.dryRun:
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()
        #out = 'Your job 1989082 ("Wjets_100to200_Part1.sh") has been submitted'
        p = re.compile("Your job (\d+) .+")
        jobNumber = p.search(out).group(1)

        jobInfo[job] = jobNumber

    with open(args.outDirectory + '/jobs.pickle', 'wb') as handle:
      pickle.dump(jobInfo, handle, protocol=pickle.HIGHEST_PROTOCOL)















