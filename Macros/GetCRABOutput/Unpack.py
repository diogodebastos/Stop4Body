#!/usr/bin/python

import os,subprocess,ROOT
#import shlex


def listDir(path):
  p = subprocess.Popen(['ls',path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
  out, err = p.communicate()
  return out.split()


def unpackPath(path, basePath, prod, label, datasets = [], listOnly = False, skim = False):
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

    for TS in timeStamps:
      if haveAllFiles:
        break;

      kSplit = listDir(basePath+'/'+prod+'/'+label+'/'+ds+'/'+TS)
      for k in kSplit:
        startPath = basePath+'/'+prod+'/'+label+'/'+ds+'/'+TS+'/'+k

        filestmp = listDir(startPath)
        files = []
        for ifile in filestmp:
          if ifile=='failed' or ifile=='log' or 'treeProducer' in ifile:
            continue
          files.append(ifile);

        failedLogFiles = listDir(startPath+'/failed/log')

        NFiles = len(files)
        maxChunk = NFiles
        for ifile in files:
          maxChunk = max(maxChunk, int(ifile[12:-4]))
        for ifile in failedLogFiles:
          maxChunk = max(maxChunk, int(ifile[7:-11]))

        allChunks = range(1, maxChunk+1)
        missingChunks = allChunks
        for ifile in files:
          missingChunks.remove(int(ifile[12:-4]))
        for i in missingChunks:
          print "  Missing chunk : ", ds, '- Chunk'+str(i)

        if len(missingChunks)==0:
          haveAllFiles=True

        if listOnly:
          continue


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

  args = parser.parse_args()
  if not args.listOnly:
    if args.path == None or args.path == '':
      print "error: argument --path is required"
      quit()

  unpackPath(path = args.path, basePath = args.basePath, prod = args.prod, label = args.label, datasets = args.dataset, listOnly = args.listOnly, skim = args.skim)
