import os,subprocess,ROOT
import pickle
import re
import time

def getNJobs():
  #cmd = "qstat | wc -l"
  cmd = "squeue -u dbastos | wc -l"
  p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = p.communicate()

  return int(out)

if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-o', '--outDirectory', required=True, help='Name of the output directory')
  parser.add_argument('-d', '--dryRun', action='store_true', help='Do a dry run (i.e. do not actually run the potentially dangerous commands but print them to the screen)')
  parser.add_argument(      '--filterCheck', default="*", help='Glob to be used when searching which samples have to be checked')
  parser.add_argument(      '--noMerge', action='store_true', help='Do not merge the output files')
  parser.add_argument('-a', '--printAll', action='store_true', help='Whether to print all jobs at the end, or only those that are still being processed')

  args = parser.parse_args()

  if not args.dryRun:
    print "You did not enable dry run. You are on your own!"

  cmd = "ls -d " + args.outDirectory + "/" + args.filterCheck + "/"
  p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = p.communicate()
  samples = out.split()

  print "Found", len(samples), "samples"

  summary = {}

  for sample in samples:
    sampleName = os.path.basename(os.path.normpath(sample))
    if sampleName[:6] == "DataMC":
      continue
    if sampleName[:7] == "2Dplots":
      continue
    if sampleName[:8] == "WJetsHT_":
      continue
    if sampleName[:7] == "WNJets_":
      continue
    if sampleName[:9] == "MCClosure":
      continue
    if sampleName[:4] == "Plot":
      continue
    if sampleName[:4] == "QCD-":
      continue
    if sampleName[:] == "ShapeComparison":
      continue
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
#      outFile = sample + "/" + job + ".o" + jobInfo[job];
#      errFile = sample + "/" + job + ".e" + jobInfo[job];
      slurmFile = sample + "/slurm-" + jobInfo[job] + ".out" 

      if os.path.isfile(slurmFile):
        if 'Done' in open(slurmFile).read():
          if os.stat(slurmFile).st_size > 1631:
            resubmitJob = False
            complete = complete + 1
          else:
            #resubmitJob = True
            resubmitJob = False #temp fix
            complete = complete + 1 #temp fix
          if 'The expected files were' in open(slurmFile).read():
            resubmitJob = True
            complete = complete - 1            
        else:
          cmd = "squeue -j " + jobInfo[job]
          p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
          out, err = p.communicate()
          if out.find("lipq") == -1:
            resubmitJob = True
          else:
            resubmitJob = False
          #resubmitJob = True
      else:
        #cmd = "qstat -j " + job
        cmd = "squeue -j " + job
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()
        print p.returncode
        if p.returncode != 0: #If the job was not found, which means it somehow stopped running and didn't produce an output file which would have been catched above
          resubmitJob = True
        else:
          resubmitJob = False

      if resubmitJob:
        #time.sleep(15)
        if args.dryRun:
          print "  Going to run the following sequence of commands:"

        cmd = "rm " + slurmFile
        if args.dryRun:
          print "\t", cmd
        else:
          p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
          out, err = p.communicate()

        fullJob = sample + "/" + job
        #cmd = "qsub -q lipq  -v CMSSW_BASE=$CMSSW_BASE " + fullJob + " -e " + fullJob + ".e$JOB_ID -o " + fullJob + ".o$JOB_ID"
        cmd = "sbatch -p lipq --export=CMSSW_BASE=$CMSSW_BASE --mem=2500 " + fullJob + " -e " + fullJob + ".e$SLURM_JOBID -o " + fullJob + ".o$SLURM_JOBID"
        if args.dryRun:
          print "\t", cmd
        else:
          p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
          out, err = p.communicate()
          #p = re.compile("Your job (\d+) .+")
          p = re.compile("Submitted batch job (\d+)")
          jobNumber = p.search(out).group(1)

          jobInfo[job] = jobNumber

    os.chdir(cwd)

    print "  Complete jobs:", complete
    summary[sampleName] = float(complete)/len(jobInfo)

    with open(sample + '/jobs.pickle', 'wb') as handle:
      pickle.dump(jobInfo, handle, protocol=pickle.HIGHEST_PROTOCOL)

#    if getNJobs() > 1700:
#      print "Waiting for some jobs to complete..."
#      while getNJobs() > 1000:
#        time.sleep(5*60)
#      print "Done waiting"

  print "Summary:"
  for sample in summary:
    if args.printAll or summary[sample] != 1.0:
      print "  ", sample, ": %.2f " % (summary[sample]*100) + "%"
  if not args.noMerge:
   for sample in summary:
    if summary[sample] == 1:
      jobInfo = {}
      with open(args.outDirectory + "/" + sample + '/jobs.pickle', 'rb') as handle:
        jobInfo = pickle.load(handle)
      cmd = "hadd " + args.outDirectory + "/" + sample + ".root"
      for jobNum in range(0, len(jobInfo)):
        jobName = sample + "_Part" + str(jobNum)
        cmd = cmd + " " + args.outDirectory + "/" + sample + "/" + jobName + ".root"
      if args.dryRun:
        print "Hadd command:", cmd
      else:
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()
