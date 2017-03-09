#!/usr/bin/python

import os,subprocess,ROOT
#import shlex


def listDir(path):
  p = subprocess.Popen(['ls',path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = p.communicate()
  return out.split()


def unpackTar(outDir, intar, parallel, skim):
  command='./untar.sh '+outDir+' '+intar+' '+("1" if skim==True else "0")
  if parallel:
    command+=(' &')
    p = subprocess.Popen(command, shell=True)
  else:
    p = subprocess.Popen(command, shell=True)
    p.wait()

def isCorrupted(path):
  corrupted = False
  f = ROOT.TFile.Open(path + '/treeProducerStop4Body/tree.root', "read")
  if not f:
    corrupted = True
  else:
    t = f.Get("tree")
    if not t:
      corrupted = True

  return corrupted


def unpackPath(path, basePath, prod, label, datasets = [], listOnly = False, skim = False, outLabel='GridProd', recoveryPath = ""):
  # Get all datasets to process
  allDatasets = listDir(basePath+'/'+prod+'/'+label)
  processedDatasets = []

  if datasets == None or datasets == []:
    processedDatasets = allDatasets
  else:
    for ds in allDatasets:
      if ds in datasets:
        processedDatasets.append(ds)

  # Get the output from each job for each dataset
  for ds in processedDatasets:
    timeStamps = listDir(basePath+'/'+prod+'/'+label+'/'+ds)
    timeStamps.sort(reverse = True)
    print ds,':\t', timeStamps

    haveAllFiles = False;
    previousHaveAllFiles = True;

    for TS in timeStamps:
      if haveAllFiles:
        break;

      kSplit = listDir(basePath+'/'+prod+'/'+label+'/'+ds+'/'+TS)
      for k in kSplit:
        previousHaveAllFiles = haveAllFiles
        haveAllFiles = False;
        startPath = basePath+'/'+prod+'/'+label+'/'+ds+'/'+TS+'/'+k

        filestmp = listDir(startPath)
        files = []
        for ifile in filestmp:
          if ifile=='failed' or ifile=='log' or 'treeProducer' in ifile:
            continue
          files.append(ifile);

        failedLogFiles = listDir(startPath+'/failed/log')

        NChunks = len(files)
        maxChunk = NChunks
        for ifile in files:
          maxChunk = max(maxChunk, int(ifile[12:-4]))
        for ifile in failedLogFiles:
          maxChunk = max(maxChunk, int(ifile[7:-11]))

        allChunks = range(1, maxChunk+1)
        missingChunks = allChunks
        for ifile in files:
          missingChunks.remove(int(ifile[12:-4]))
        for i in missingChunks:
          print "    Missing chunk : ", ds, '- Chunk'+str(i)

        if listOnly:
          if len(missingChunks) == 0 && previousHaveAllFiles == True:
            haveAllFiles = True
          continue

        # Start counters
        n = len(listDir(path+'/'+outLabel+'/'))

        if recoveryPath != None and recoveryPath != "":
          for i in missingChunks:
            outFileName = path+'/'+outLabel+'/'+ds+'/Chunk'+str(i)
            print "Checking recovery ", ds+' - Chunk'+str(i), " ===>> ", len(listDir(recoveryPath+"/"+ds+'/Chunk'+str(i))) != 0
            if len(listDir(recoveryDir+"/"+ds+'/Chunk'+str(i))) != 0:
              if len(listDir(outFileName)) != 0:
                continue
              if isCorrupted(recoveryDir+"/"+ds+'/Chunk'+str(i)):
                print "ERROR: recovery file "+ds+'/Chunk'+str(i)+" corrupted"
                continue
              os.system("cp -r "+recoveryDir+"/"+ds+'/Chunk'+str(i)+' '+outFileName)
              if skim:
                subprocess.check_call(['root','-l','-b','-q','Skimmer.C+("'+outFileName+'")'])
                subprocess.check_call(['mv', outFileName+'/treeProducerStop4Body/tree_skimmed.root', outFileName+'/treeProducerStop4Body/tree.root'])
              NChunks += 1
              n += 1

        # Unpack all files that have not been unpacked already
        for ifile in files:
          n += 1
          num = ifile[12:-4]
          outFileName = path+'/'+outLabel+'/'+ds+'/Chunk'+num
          if len(listDir(outFileName)) != 0:
            continue

          print '    '+ifile,' -> ',n,'/',NChunks
          if n%5!=0 and n!=NChunks:
            unpackTar(outFileName, startPath+'/'+ifile, True, skim) # Unpack in parellel every 5 files
          else:
            unpackTar(outFileName, startPath+'/'+ifile, False, skim)

        # Check for corruption in unpacked files and if found, try to unpack again
        for ifile in files:
          num = ifile[12:-4]
          outFileName = path+'/'+outLabel+'/'+ds+'/Chunk'+num
          corrupted = isCorrupted(outFileName)
          if corrupted:
            os.system("rm -r " + outFileName)
            unpackTar(outFileName, startPath+'/'+ifile, False, skim)
            corrupted = isCorrupted(outFileName)
            if corrupted:
              print "    ERROR : chunk "+ds+' - Chunk'+num+'is corrupted'

        nUnTar = len(ls(path+'/'+outLabel+'/'+ds))
        if nUnTar == maxFiles:
          haveAllFiles = True


if __name__ == "__main__":
  import argparse

  parser = argparse.ArgumentParser(description='Process the command line options')
  parser.add_argument('-l', '--listOnly', help='List only the missing chunks',      action='store_true')
  parser.add_argument('-s', '--skim',     help='Enable skimming',                   action='store_true')
  parser.add_argument(      '--basePath', help='Base path',                                              required=True)
  parser.add_argument(      '--prod',     help='Production version',                                     required=True)
  parser.add_argument(      '--label',    help='Production label',                                       required=True)
  parser.add_argument(      '--dataset',  help='Process specified dataset dataset', action='append')
  parser.add_argument(      '--path',     help='Save path') #Only required if not listing... so has to be done in code
  parser.add_argument(      '--outputLabel', default='GridProd', help='Label to use for the directory where output will be placed')
  parser.add_argument('--recovery', help='Recovery path')
  # TODO: Add an option to copy back to storage

  args = parser.parse_args()
  if not args.listOnly:
    if args.path == None or args.path == '':
      print "error: argument --path is required"
      quit()

  unpackPath(path = args.path, basePath = args.basePath, prod = args.prod, label = args.label, datasets = args.dataset, listOnly = args.listOnly, skim = args.skim, outLabel = args.outputLabel, recoveryPath = args.recovery)
