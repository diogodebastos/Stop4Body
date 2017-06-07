import os,subprocess,ROOT
import pickle
import re

if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-o', '--outDirectory', required=True, help='Name of the output directory')
  parser.add_argument('-d', '--dryRun', action='store_true', help='Do a dry run (i.e. do not actually run the potentially dangerous commands but print them to the screen)')

  args = parser.parse_args()

  cmd = "ls -d " + args.outDirectory + "/*/"
  p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = p.communicate()
  samples = out.split()

  print "Found", len(samples), "samples"

  summary = {}

  for sample in samples:
    sampleName = os.path.basename(os.path.normpath(sample))
    print "Sample:", sampleName

    jobInfo = {}
    with open(sample + '/jobs.pickle', 'rb') as handle:
      jobInfo = pickle.load(handle)
    print "  Found", len(jobInfo), "jobs"

    cwd = os.getcwd() # for now, we need to change the dorectory so that the out and err files are in the right place
    os.chdir(sample)

    complete = 0
    for job in jobInfo:
      resubmitJob = False
      outFile = sample + "/" + job + ".o" + jobInfo[job];
      errFile = sample + "/" + job + ".e" + jobInfo[job];

      if os.path.isfile(outFile) and os.path.isfile(errFile):
        if 'Done' in open(outFile).read():
          resubmitJob = False
          complete = complete + 1 #TODO: add check for whether there were any errors in the output itself
        else:
          resubmitJob = True
      else:
        resubmitJob = False #TODO: here we need to add a check if the job is actually running

      if resubmitJob:
        if args.dryRun:
          print "Going to run the following sequence of commands:"

        cmd = "rm " + outFile
        if args.dryRun:
          print "\t", cmd
        else:
          p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
          out, err = p.communicate()

        cmd = "rm " + errFile
        if args.dryRun:
          print "\t", cmd
        else:
          p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
          out, err = p.communicate()

        fullJob = sample + "/" + job
        cmd = "qsub " + fullJob + " -e " + fullJob + ".e$JOB_ID -o " + fullJob + ".o$JOB_ID"
        if args.dryRun:
          print "\t", cmd
        else:
          p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
          out, err = p.communicate()
          p = re.compile("Your job (\d+) .+")
          jobNumber = p.search(out).group(1)

          jobInfo[job] = jobNumber

    os.chdir(cwd)

    print "  Complete jobs:", complete
    summary[sampleName] = float(complete)/len(jobInfo)

    with open(sample + '/jobs.pickle', 'wb') as handle:
      pickle.dump(jobInfo, handle, protocol=pickle.HIGHEST_PROTOCOL)

  print "Summary:"
  for sample in summary:
    print "  ", sample, ": ", summary[sample]*100, "%"
  for sample in summary:
    if summary[sample] == 1:
      cmd = "hadd " + args.outDirectory + "/" + sample + ".root"
      for job in jobInfo:
        jobName = os.path.splitext(job)[0]
        cmd = cmd + " " + args.outDirectory + "/" + sample + "/" + jobName + ".root"
      if args.dryRun:
        print "Hadd command:", cmd
      else:
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()







