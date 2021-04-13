import os
# import shutil

def assure_path_exists(path):
  dir = os.path.dirname(path)
  if not os.path.exists(dir):
    os.makedirs(dir)

def createJSON(stop,neutralino,year):
    stop = str(stop)
    neutralino = str(neutralino)
    outputDirectory = "tmp/"
    if year == "2017":
        outputDirectory = "JSON/2017/Orig/SignalPoints/"
    elif year == "2018":
        outputDirectory = "JSON/2018/Orig/SignalPoints/"
    assure_path_exists(outputDirectory)
    outputDirectory = os.path.realpath(outputDirectory)

    with open(outputDirectory + "/Sig_"+stop+"_"+neutralino+".json", 'w') as thisJSON:
        thisJSON.write("{\n")
        thisJSON.write("  \"lines\":[\n")
        thisJSON.write("    {\n")
        thisJSON.write("      \"tag\":\"signal_point_"+stop+"_"+neutralino+"\",\n")
        thisJSON.write("      \"isdata\":false,\n")
        thisJSON.write("      \"issignal\":true,\n")
        thisJSON.write("      \"isfastsim\":true,\n")
        thisJSON.write("      \"spimpose\":true,\n")
        thisJSON.write("      \"color\":616,\n")
        thisJSON.write("      \"lwidth\":4,\n")
        thisJSON.write("      \"lstyle\":1,\n")
        thisJSON.write("      \"fill\":0,\n")
        thisJSON.write("      \"marker\":1,\n")
        thisJSON.write("      \"label\":\"signal=("+stop+","+neutralino+")\",\n")
        thisJSON.write("      \"selection\":\"(GenSusyMStop=="+stop+")&&(GenSusyMNeutralino=="+neutralino+")\",\n")
        thisJSON.write("      \"files\":[\n")
        thisJSON.write("        {\n")
        thisJSON.write("          \"tag\":\"T2DegStop_"+stop+"_"+neutralino+"\",\n")
        thisJSON.write("          \"xsec\": 1.0,\n")
        thisJSON.write("          \"br\": 1.0,\n")
        thisJSON.write("          \"filesPerPart\": 25,\n")
        #thisJSON.write("          \"filesPerPart\": 50,\n")
        thisJSON.write("          \"filterEfficiencyFile\":\"../data/filterEffs_SMS_T2tt_dM_10to80_genHT_160_genMET_80_mWMin_0p1.root\",\n")
        thisJSON.write("          \"paths\":[\n")
        thisJSON.write("            {\n")
        if year == "2017":
            thisJSON.write("              \"path\":\"/gstore/t3cms/store/user/dchasque/nanoAOD/Stop4Body17/Fall17_Prod2020Apr20/SMS_T2tt_dM_10to80_genHT_160_genMET_80_mWMin_0p1\",\n")
            thisJSON.write("              \"split\":60\n")
        elif year == "2018":
            thisJSON.write("              \"path\":\"/gstore/t3cms/store/user/dchasque/nanoAOD/Stop4Body18/Autumn18_Prod2020Jan06/SMS_T2tt_dM_10to80_genHT_160_genMET_80_mWMin_0p1\",\n")
            thisJSON.write("              \"split\":107\n")
        thisJSON.write("            }\n")
        thisJSON.write("          ]\n")
        thisJSON.write("        }\n")
        thisJSON.write("      ]\n")
        thisJSON.write("    }\n")
        thisJSON.write("  ]\n")
        thisJSON.write("}\n")

YEAR = os.getenv("YEAR")
fixStop = 0
dixDM = 0
for stop in range(250, 1101, 25):
    fixStop = stop
    for dm in range(80,0,-10):
        fixDM = dm
        if stop == 525:
            if dm > 10:
                fixDM = dm + 1
            fixStop = 526
        elif stop == 575:
            if dm > 60:
                fixDM = dm + 1
            fixStop = 576
        elif stop == 625:
            fixStop = 626
        elif stop == 675:
            fixStop = 676
        elif stop == 725:
            fixStop = 726
        elif stop == 775:
            fixStop = 776
        elif stop == 825:
            fixStop = 826
        elif stop == 875:
            fixStop = 876
        elif stop == 925:
            fixStop = 926
        elif stop == 975:
            fixStop = 976
        elif stop == 1025:
            fixDM = dm - 2
            fixStop = 1024
        elif stop == 1050:
            if dm != 20:
                fixDM = dm + 2
            fixStop = 1052
        elif stop == 1075:
            if dm == 10 or dm == 30 or dm == 50:
                fixDM = dm + 2
            fixStop = 1076
        elif stop == 1100:
            if dm == 10 or dm == 30 or dm == 50 or dm == 70:
                fixDM = dm - 2

        neutralino = fixStop-fixDM
        createJSON(fixStop,neutralino,YEAR)
