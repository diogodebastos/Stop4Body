import ROOT
import os, sys

print 'Merging %s' % sys.argv[1]

print "Max tree size",ROOT.TTree.GetMaxTreeSize()
ROOT.TTree.SetMaxTreeSize(200000000000) # 200 Gb
print "Updated tree size",ROOT.TTree.GetMaxTreeSize()

rm = ROOT.TFileMerger(False)
rm.SetFastMethod(True)


#path = '/lstore/cms/dbastos/Stop4Body/nTuples18_nanoAOD_v2020-10-25-WJetsNLO/WJetsToLNu_Pt_250To400/'
#path = '/lstore/cms/dbastos/Stop4Body/nTuples18_nanoAOD_v2020-11-03/WJetsToLNu_Pt_250To400/'
path = '/lstore/cms/dbastos/Stop4Body/nTuples18_nanoAOD_v2020-11-03-WJetsNLO/WJetsToLNu_Pt_250To400/'
path = '/lstore/cms/dbastos/Stop4Body/nTuples18_nanoAOD_v2020-11-03-clJetLep_isPFcand-WJetsNLO/WJetsToLNu_Pt_250To400/'
path = '/lstore/cms/dbastos/Stop4Body/nTuples18_nanoAOD_v2020-11-11/WJetsToLNu_Pt_250To400/'
path = '/lstore/cms/dbastos/Stop4Body/nTuples18_nanoAOD_v2020-11-11-WJetsNLO/WJetsToLNu_Pt_250To400/'
path = '/lstore/cms/dbastos/Stop4Body/nTuples17_nanoAOD_v2021-01-25-forFakeRate/TTJets/'
path = '/lstore/cms/dbastos/Stop4Body/nTuples17_nanoAOD_v2021-02-03-forFakeRate/TTJets/'
path = '/lstore/cms/dbastos/Stop4Body/nTuples17_nanoAOD_v2021-02-04-forFakeRate/TTJets/'
#path = "./"
file_output = '%s.root' % sys.argv[1]
file_list = []
for path, dirs, files in os.walk(path):
  for filename in files:
    #print filename
    #print ('%s_Part' % sys.argv[1])
    if ('%s_Part' % sys.argv[1]) in filename and ".root" in filename: file_list.append(path+filename)

print "Input file list:",file_list
print "Output file:",file_output

for F in file_list:

    print "Adding ->",F
    rm.AddFile(F)

rm.OutputFile(path+file_output)
rm.Merge()
