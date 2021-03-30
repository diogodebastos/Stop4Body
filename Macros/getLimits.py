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

def submitJobs(inputDirectory, outputDirectory, fullCLs=False, unblind=False, dryRun=True):
  for deltaM in (10, 20, 30, 40, 50, 60, 70, 80):
    for stopM in (250, 275, 300, 325, 350, 375, 400, 425, 450, 475, 500, 525, 550, 575, 600, 625, 650, 675, 700, 725, 750, 775, 800):
  #tmpDM=[10]
  #for deltaM in tmpDM:
    #for stopM in (250, 275, 300, 325):
      neutM = stopM - deltaM
      datacardName = str(stopM) + "_" + str(neutM) + ".txt"

      assure_path_exists(outputDirectory + "/" + str(stopM) + "_" + str(neutM) + "/")

      cmd = "cp " + inputDirectory + "/" + datacardName + " " + outputDirectory + "/" + str(stopM) + "_" + str(neutM) + "/"
      print cmd
      p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
      out, err = p.communicate()
      
      cwd = os.getcwd()
      job = outputDirectory + "/" + str(stopM) + "_" + str(neutM) + "/job_" + str(stopM) + "_" + str(neutM) + ".sh"
      tmpDir=outputDirectory + "/" + str(stopM) + "_" + str(neutM) + "/"
      os.chdir(tmpDir)
      with open(job, 'w') as thisScript:
        thisScript.write("#!/bin/bash\n\n")
        
        # 2016
        thisScript.write("alias cmsenv='eval `scramv1 runtime -sh`'\n\n")

        thisScript.write("cd /exper-sw/cmst3/cmssw/users/cbeiraod/\n")
        thisScript.write(". setup.sh\n\n")

        thisScript.write("cd $CMSSW_BASE/src/\n")
        thisScript.write("cd /exper-sw/cmst3/cmssw/users/cbeiraod/Stop4Body/CMSSW_8_1_0/src/\n")
        thisScript.write("eval `scramv1 runtime -sh`\n\n")

        # 2017 and 2018
        #thisScript.write("alias cmsenv='eval `scramv1 runtime -sh`'\n\n")

        #thisScript.write("export SCRAM_ARCH=slc7_amd64_gcc700\n")
        #thisScript.write("export VO_CMS_SW_DIR=/cvmfs/cms.cern.ch\n")
        #thisScript.write("source $VO_CMS_SW_DIR/cmsset_default.sh\n")
        #thisScript.write("export CMS_PATH=$VO_CMS_SW_DIR\n")
        #thisScript.write("source /cvmfs/cms.cern.ch/crab3/crab.sh\n")
        #thisScript.write("cd /lstore/cms/dbastos/REPOS/CMSSW_10_2_13/src/\n")
        #thisScript.write("eval `scramv1 runtime -sh`\n\n")

        thisScript.write("cd " + outputDirectory + "/" + str(stopM) + "_" + str(neutM) + "\n\n")

        # First get the limits without fitting to data, the a-priori limits
        # Should be used before unblinding
        thisScript.write("combine -M AsymptoticLimits --run=blind -n APriori") # before was --noFitAsimov
        thisScript.write(" " + datacardName)
        thisScript.write(" > aPriori.txt\n")

        thisScript.write("\n")

        # Then generate a background only asimov toy and perform a maximum likelihood fit
        # The objective is to take a look at the pulls of the nuisance parameters and identify possible problems
#        thisScript.write("combine -M FitDiagnostics --forceRecreateNLL -t -1")
#        thisScript.write(" -n BOnly")
#        thisScript.write(" --expectSignal 0 " + datacardName)
#        thisScript.write(" > Bonly.txt\n")

        # Get the pulls of the nuisance parameters
#        thisScript.write("python $CMSSW_BASE/src/HiggsAnalysis/CombinedLimit/test/diffNuisances.py")
#        thisScript.write(" -a fitDiagnosticsBOnly.root")
#        thisScript.write(" -g plots_Bonly.root")
#        thisScript.write(" > Bonly_pulls.txt\n")

#        thisScript.write("\n")

        # Repeat for B+S
#        thisScript.write("combine -M FitDiagnostics --forceRecreateNLL -t -1")
#        thisScript.write(" -n BpS")
#        thisScript.write(" --expectSignal 1 " + datacardName)
#        thisScript.write(" > BpS.txt\n")

#        thisScript.write("python $CMSSW_BASE/src/HiggsAnalysis/CombinedLimit/test/diffNuisances.py")
#        thisScript.write(" -a fitDiagnosticsBpS.root")
#        thisScript.write(" -g plots_BpS.root")
#        thisScript.write(" > BpS_pulls.txt\n")

#        thisScript.write("\n")

        # Run the full CLs method
        # We need to run independently for each quantile
        if fullCLs:
          thisScript.write("combine -M HybridNew --LHCmode LHC-limits -t -1")
          thisScript.write(" --expectedFromGrid 0.5")
          thisScript.write(" -n APriori")
          thisScript.write(" " + datacardName)
          thisScript.write(" > aPrioriFullCLs0500.txt\n")

          thisScript.write("combine -M HybridNew --LHCmode LHC-limits -t -1")
          thisScript.write(" --expectedFromGrid 0.16")
          thisScript.write(" -n APriori")
          thisScript.write(" " + datacardName)
          thisScript.write(" > aPrioriFullCLs0160.txt\n")

          thisScript.write("combine -M HybridNew --LHCmode LHC-limits -t -1")
          thisScript.write(" --expectedFromGrid 0.84")
          thisScript.write(" -n APriori")
          thisScript.write(" " + datacardName)
          thisScript.write(" > aPrioriFullCLs0840.txt\n")

          thisScript.write("combine -M HybridNew --LHCmode LHC-limits -t -1")
          thisScript.write(" --expectedFromGrid 0.975")
          thisScript.write(" -n APriori")
          thisScript.write(" " + datacardName)
          thisScript.write(" > aPrioriFullCLs0975.txt\n")

          thisScript.write("combine -M HybridNew --LHCmode LHC-limits -t -1")
          thisScript.write(" --expectedFromGrid 0.025")
          thisScript.write(" -n APriori")
          thisScript.write(" " + datacardName)
          thisScript.write(" > aPrioriFullCLs0025.txt\n")

          thisScript.write("hadd higgsCombineAPriori.HybridNew.root higgsCombineAPriori.HybridNew.mH120*.root\n")

          thisScript.write("\n")

        # Then get the a-posteriori limits, can only be retrieved after unblinding
        if unblind:
          thisScript.write("\n")
          thisScript.write("\n")
          thisScript.write("combine -M AsymptoticLimits -n APosteriori")
          thisScript.write(" " + datacardName)
          thisScript.write(" > aPosteriori.txt\n")

          if fullCLs:
            thisScript.write("combine -M HybridNew --LHCmode LHC-limits")
            thisScript.write(" --expectedFromGrid 0.5")
            thisScript.write(" -n APosteriori")
            thisScript.write(" " + datacardName)
            thisScript.write(" > aPosterioriFullCLs0500.txt\n")

            thisScript.write("combine -M HybridNew --LHCmode LHC-limits")
            thisScript.write(" --expectedFromGrid 0.84")
            thisScript.write(" -n APosteriori")
            thisScript.write(" " + datacardName)
            thisScript.write(" > aPosterioriFullCLs0840.txt\n")

            thisScript.write("combine -M HybridNew --LHCmode LHC-limits")
            thisScript.write(" --expectedFromGrid 0.16")
            thisScript.write(" -n APosteriori")
            thisScript.write(" " + datacardName)
            thisScript.write(" > aPosterioriFullCLs0160.txt\n")

            thisScript.write("combine -M HybridNew --LHCmode LHC-limits")
            thisScript.write(" --expectedFromGrid 0.975")
            thisScript.write(" -n APosteriori")
            thisScript.write(" " + datacardName)
            thisScript.write(" > aPosterioriFullCLs0975.txt\n")

            thisScript.write("combine -M HybridNew --LHCmode LHC-limits")
            thisScript.write(" --expectedFromGrid 0.025")
            thisScript.write(" -n APosteriori")
            thisScript.write(" " + datacardName)
            thisScript.write(" > aPosterioriFullCLs0025.txt\n")

            thisScript.write("combine -M HybridNew --LHCmode LHC-limits")
            thisScript.write(" -n APosteriori")
            thisScript.write(" " + datacardName)
            thisScript.write(" > aPosterioriFullCLs.txt\n")

            thisScript.write("hadd higgsCombineAPosteriori.HybridNew.root higgsCombineAPosteriori.HybridNew.mH120*.root\n")

        mode = os.fstat(thisScript.fileno()).st_mode
        mode |= 0o111
        os.fchmod(thisScript.fileno(), mode & 0o7777)

      cmd = "qsub " + job + " -e " + job + ".e$JOB_ID -o " + job + ".o$JOB_ID"
      #cmd = "qsub -q lipq " + job + " -e " + job + ".e$JOB_ID -o " + job + ".o$JOB_ID"
      #cmd = "qsub -q lipq -v CMSSW_BASE=$CMSSW_BASE " + job + " -e " + job + ".e$JOB_ID -o " + job + ".o$JOB_ID"
      print cmd
      if not dryRun:
        p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        out, err = p.communicate()

        if getNJobs() > 1700:
          print "Waiting for some jobs to complete..."
          while getNJobs() > 1000:
            time.sleep(5*60)
          print "Done waiting"
      os.chdir(cwd)

def collectJobs(outputDirectory, fullCLs=False, unblind=False):
  doneProcessing = True

  for deltaM in (10, 20, 30, 40, 50, 60, 70, 80):
    for stopM in (250, 275, 300, 325, 350, 375, 400, 425, 450, 475, 500, 525, 550, 575, 600, 625, 650, 675, 700, 725, 750, 775, 800):
      neutM = stopM - deltaM
      pointDirectory = outputDirectory + "/" + str(stopM) + "_" + str(neutM) + "/"
      if not (os.path.exists(pointDirectory) and os.path.isdir(pointDirectory)):
        print "The directory \"" + pointDirectory + "\" does not exist, there must have been some problem, aborting."
        doneProcessing = False
        continue

      # Add checks for whether the jobs successfully terminated
      #

  if not doneProcessing:
    return 1

  aPrioriAsymp = {}
  aPosterioriAsymp = {}
  aPrioriFullCLs = {}
  aPosterioriFullCLs = {}
  aPrioriAsympDM = {}
  aPosterioriAsympDM = {}
  aPrioriFullCLsDM = {}
  aPosterioriFullCLsDM = {}

  for stopM in (250, 275, 300, 325, 350, 375, 400, 425, 450, 475, 500, 525, 550, 575, 600, 625, 650, 675, 700, 725, 750, 775, 800):
    aPrioriAsymp[stopM] = {}
    aPrioriAsympDM[stopM] = {}
    if fullCLs:
      aPrioriFullCLs[stopM] = {}
      aPrioriFullCLsDM[stopM] = {}
    if unblind:
      aPosterioriAsymp[stopM] = {}
      aPosterioriAsympDM[stopM] = {}
      if fullCLs:
        aPosterioriFullCLs[stopM] = {}
        aPosterioriFullCLsDM[stopM] = {}
    for deltaM in (10, 20, 30, 40, 50, 60, 70, 80):
      neutM = stopM - deltaM
      aPrioriAsymp[stopM][neutM] = {}
      aPrioriAsympDM[stopM][deltaM] = {}
      if fullCLs:
        aPrioriFullCLs[stopM][neutM] = {}
        aPrioriFullCLsDM[stopM][deltaM] = {}
      if unblind:
        aPosterioriAsymp[stopM][neutM] = {}
        aPosterioriAsympDM[stopM][deltaM] = {}
        if fullCLs:
          aPosterioriFullCLs[stopM][neutM] = {}
          aPosterioriFullCLsDM[stopM][deltaM] = {}

  for deltaM in (10, 20, 30, 40, 50, 60, 70, 80):
    for stopM in (250, 275, 300, 325, 350, 375, 400, 425, 450, 475, 500, 525, 550, 575, 600, 625, 650, 675, 700, 725, 750, 775, 800):
      neutM = stopM - deltaM
      pointDirectory = outputDirectory + "/" + str(stopM) + "_" + str(neutM) + "/"

      print "Getting results from:"
      print "  Mstop=", stopM
      print "  Mneut=", neutM

      # DM 10 and Stop 250-325 the yields are too big so we divide the only the signal yield by  factor of 0.1x and multiply r later by the same factor of 0.1x
      factor = 1
      if deltaM == 10:
        if stopM == 250 or stopM == 275 or stopM == 300 or stopM == 325:
          factor = 0.1

      # Get the Asymptotic results:
      asympInFile = ROOT.TFile(pointDirectory + "higgsCombineAPriori.AsymptoticLimits.mH120.root", "READ")
      asympTree = asympInFile.Get("limit")
      if asympTree:
        for limit in asympTree:
          aPrioriAsymp[stopM][neutM]['{0:.3f}'.format(limit.quantileExpected)] = limit.limit * factor
          aPrioriAsympDM[stopM][deltaM]['{0:.3f}'.format(limit.quantileExpected)] = limit.limit * factor

      # If processing the fullCLs, get its results too:
      if fullCLs:
        fullCLsInFile = ROOT.TFile(pointDirectory + "higgsCombineAPriori.HybridNew.root", "READ")
        fullCLsTree = fullCLsInFile.Get("limit")
        if fullCLsTree:
          for limit in fullCLsTree:
            aPrioriFullCLs[stopM][neutM]['{0:.3f}'.format(limit.quantileExpected)] = limit.limit * factor
            aPrioriFullCLsDM[stopM][deltaM]['{0:.3f}'.format(limit.quantileExpected)] = limit.limit * factor
      print "factor: " + str(factor)
      # Repeat the above for the case we have unblinded
      if unblind:
        asympInFile = ROOT.TFile(pointDirectory + "higgsCombineAPosteriori.AsymptoticLimits.mH120.root", "READ")
        asympTree = asympInFile.Get("limit")
        if asympTree:
          for limit in asympTree:
            aPosterioriAsymp[stopM][neutM]['{0:.3f}'.format(limit.quantileExpected)] = limit.limit * factor
            aPosterioriAsympDM[stopM][deltaM]['{0:.3f}'.format(limit.quantileExpected)] = limit.limit * factor

        if fullCLs:
          fullCLsInFile = ROOT.TFile(pointDirectory + "higgsCombineAPosteriori.HybridNew.root", "READ")
          fullCLsTree = fullCLsInFile.Get("limit")
          if fullCLsTree:
            for limit in fullCLsTree:
              aPosterioriFullCLs[stopM][neutM]['{0:.3f}'.format(limit.quantileExpected)] = limit.limit * factor
              aPosterioriFullCLsDM[stopM][deltaM]['{0:.3f}'.format(limit.quantileExpected)] = limit.limit * factor

  import pickle

  pickle.dump( aPrioriAsymp, open( outputDirectory + "/aPrioriAsymp.pkl", "wb" ) )
  pickle.dump( aPrioriAsympDM, open( outputDirectory + "/aPrioriAsympDM.pkl", "wb" ) )
  if fullCLs:
    pickle.dump( aPrioriFullCLs, open( outputDirectory + "/aPrioriFullCLs.pkl", "wb" ) )
    pickle.dump( aPrioriFullCLsDM, open( outputDirectory + "/aPrioriFullCLsDM.pkl", "wb" ) )
  if unblind:
    pickle.dump( aPosterioriAsymp, open( outputDirectory + "/aPosterioriAsymp.pkl", "wb" ) )
    pickle.dump( aPosterioriAsympDM, open( outputDirectory + "/aPosterioriAsympDM.pkl", "wb" ) )
    if fullCLs:
      pickle.dump( aPosterioriFullCLs, open( outputDirectory + "/aPosterioriFullCLs.pkl", "wb" ) )
      pickle.dump( aPosterioriFullCLsDM, open( outputDirectory + "/aPosterioriFullCLsDM.pkl", "wb" ) )

  return 0

if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-i', '--inputDirectory', required=True, help='Name of the input directory')
  parser.add_argument('-o', '--outputDirectory', required=True, help='Base name of the output directory for each DM')
  parser.add_argument('-d', '--dryRun', action='store_true', help='Do a dry run (i.e. do not actually run the potentially dangerous commands but print them to the screen)')
  parser.add_argument('-u', '--unblind', action='store_true', help='Whether the results have been unblinded or not')
  parser.add_argument('-f', '--fullCLs', action='store_true', help='Whether to run the full CLs method or not')
  parser.add_argument('-s', '--submit', action='store_true', help='Whether to build and submit the jobs to calculate the limits')
  parser.add_argument('-c', '--collect', action='store_true', help='Whether to collect the output of the jobs previously submitted')

  args = parser.parse_args()

  inputDirectory = args.inputDirectory
  if args.submit and not (os.path.exists(inputDirectory) and os.path.isdir(inputDirectory)):
    parser.error('The given input directory does not exist or is not a directory')

  if not args.submit and not args.collect:
    parser.error('You must define either to submit the jobs or collect the job outputs')

  if args.submit and args.collect:
    parser.error('You can not simultaneously submit the jobs and collect their outputs')

  if args.submit and not args.dryRun:
    print "You did not enable dry run. You are on your own!"

  baseDirectory = os.path.realpath(os.getcwd())
  outputDirectory = args.outputDirectory
  assure_path_exists(outputDirectory)

  if args.submit:
    inputDirectory = os.path.realpath(inputDirectory)
    submitJobs(inputDirectory, outputDirectory, fullCLs=args.fullCLs, unblind=args.unblind, dryRun=args.dryRun)
  if args.collect:
    collectJobs(outputDirectory, fullCLs=args.fullCLs, unblind=args.unblind)


















