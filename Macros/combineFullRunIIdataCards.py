import os,subprocess
path = "/lstore/cms/dbastos/REPOS/Stop4Body-nanoAOD/CMSSW_8_0_14/src/UserCode/Stop4Body-nanoAOD/Macros/DataCards/"

for deltaM in (10, 20, 30, 40, 50, 60, 70, 80):
	for stopM in (250, 275, 300, 325, 350, 375, 400, 425, 450, 475, 500, 525, 550, 575, 600, 625, 650, 675, 700, 725, 750, 775, 800):
		neutM = stopM - deltaM
		datacardName = str(stopM) + "_" + str(neutM) + ".txt"

		cmd = "combineCards.py " + path + "/2016/" + datacardName + " " + path + "/2017/" + datacardName + path + "/2018/" + datacardName + " > " + path + "/FullRunII/" + datacardName

		print cmd

		#p = subprocess.Popen(cmd, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

		#out, err = p.communicate()