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
  parser.add_argument('-j', '--jsonTemplate', default="./Templates/jsonTemplate.json", help='Template for the json file for each job')
  parser.add_argument('-t', '--jobTemplate', default="./Templates/Step1_JobTemplate.sh", help='Template for the script for each job')
  parser.add_argument(      '--filterSubmit', default="*", help='Glob to be used when searching which samples have to be submitted')
  parser.add_argument('-s', '--doSwap', action='store_true', help='Whether to process with the swapping of MET and LepPt variables')
  parser.add_argument('-y', '--year', required=True, help='The year the samples were produced.')
  parser.add_argument('-d', '--dryRun', action='store_true', help='Do a dry run (i.e. do not actually run the potentially dangerous commands but print them to the screen)')
  parser.add_argument(      '--onlyData', action='store_true', help='Whether to only run on data')
  parser.add_argument(      '--verbose', action='store_true', help='Give verbose output')
  parser.add_argument(      '--thisSample', help='Build jobs only for a specific sample')


  args = parser.parse_args()

  if not args.dryRun:
    print "You did not enable dry run. You are on your own!"

  cmd = "./setupJSONs.sh"
  p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = p.communicate()

  jsonFiles = []
  if args.doSwap:
    jsonFiles.append("DataSingleLepton.json")
  else:
    print "Data"
    #jsonFiles.append("Data.json")
    #jsonFiles.append("analysisData.json")
#    jsonFiles.append("DataMetHT.json")
#    jsonFiles.append("DataJetHT.json")
  if not args.onlyData:
    print "MC"
#    jsonFiles.append("MC2Process.json")
#    jsonFiles.append("AllSignals.json")
#    jsonFiles.append("TTbar.json")
#    jsonFiles.append("WJetsNLO.json")
#    jsonFiles.append("ZInv.json")
#    jsonFiles.append("otherMC1.json")
#    jsonFiles.append("otherMC2.json")
    #jsonFiles.append("Orig/SingleTop.json")
    #jsonFiles.append("Orig/QCD.json")
    #jsonFiles.append("Orig/QCD_Mu.json")
    #jsonFiles.append("Orig/VV.json")
    #jsonFiles.append("Orig/WJetsHT.json")
    #jsonFiles.append("Orig/WNJets.json")
    #jsonFiles.append("Orig/Wjets-HT1200To2500.json")
    #jsonFiles.append("Orig/TTWToLNu.json")
    #jsonFiles.append("Orig/TTZToLLNuNu.json")
    #jsonFiles.append("tmpMC.json")
    jsonFiles.append("stop500.json")
    jsonFiles.append("stop250.json")
    jsonFiles.append("stop275.json")
    jsonFiles.append("stop300.json")
    jsonFiles.append("stop325.json")
    jsonFiles.append("stop350.json")
    jsonFiles.append("stop375.json")
    jsonFiles.append("stop400.json")
    jsonFiles.append("stop425.json")
    jsonFiles.append("stop450.json")
    jsonFiles.append("stop475.json")
    #jsonFiles.append("stop500_FullSim.json")
    jsonFiles.append("stop525.json")
    jsonFiles.append("stop550.json")
    jsonFiles.append("stop575.json")
    jsonFiles.append("stop600.json")
    jsonFiles.append("stop625.json")
    jsonFiles.append("stop650.json")
    jsonFiles.append("stop675.json")
    jsonFiles.append("stop700.json")
    jsonFiles.append("stop725.json")
    jsonFiles.append("stop750.json")
    jsonFiles.append("stop775.json")
    jsonFiles.append("stop800.json")
  if args.thisSample:
    jsonFiles = []
    jsonFiles.append(args.thisSample+".json")

  cmd = "buildJobs --template " + args.jobTemplate + " --jsonTemplate " + args.jsonTemplate + " --outDir " + args.outDirectory
  if args.doSwap:
    cmd = cmd + " --swap"

  for json in jsonFiles:
    thisCMD = cmd + " --samples JSON/" + args.year + "/" + json
    print "Running the command:", thisCMD
    p = subprocess.Popen(thisCMD, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()

  cmd = "ls -d " + args.outDirectory + "/" + args.filterSubmit + "/"
  p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = p.communicate()
  samples = out.split()

  for sample in samples:
    sampleName = os.path.basename(os.path.normpath(sample))
    print "Processing sample:", sampleName
    cmd = "ls " + sample + "/*.sh"
    p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    jobs = out.split()

    cwd = os.getcwd() # for now, we need to change the directory so that the out and err files are in the right place
    os.chdir(sample)

    jobInfo = {}
    for job in jobs:
      jobName = os.path.basename(job)
      #cmd = "qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE " + job + " -e " + job + ".e$JOB_ID -o " + job + ".o$JOB_ID"
      cmd = "sbatch -p lipq --export=CMSSW_BASE=$CMSSW_BASE --mem=2500 " + job + " -e " + job + ".e$SLURM_JOBID -o " + job + ".o$SLURM_JOBID"
      if args.dryRun:
        print "Going to run command:", cmd
      if not args.dryRun:
        #time.sleep(5)
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()
        #Your job 6137168 ("T2DegStop_576_505_Part0.sh") has been submitted
        #p = re.compile("Your job (\d+) .+")
        #Submitted batch job 719018
        p = re.compile("Submitted batch job (\d+)")
        if args.verbose:
          print "CMD:", cmd
          print "OUT:", out
          print "ERR:", err
        jobNumber = p.search(out).group(1)

        jobInfo[jobName] = jobNumber

    os.chdir(cwd)

    with open(sample + '/jobs.pickle', 'wb') as handle:
      pickle.dump(jobInfo, handle, protocol=pickle.HIGHEST_PROTOCOL)

    if getNJobs() > 1700:
      print "Waiting for some jobs to complete..."
      while getNJobs() > 1000:
        time.sleep(5*60)
      print "Done waiting"
