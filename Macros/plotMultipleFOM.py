import os, csv, operator
from matplotlib import pyplot as plt
import numpy as np

def fomFromFile(dirHT):
    files = os.listdir(dirHT)
    for file in files:
        if not file[-11:]=="eff_fom.csv": continue
        bdt=[]
        fom=[]
        with open(dirHT+file) as f:
            reader = csv.reader(f)
            header_row = next(reader)
            for index,column_header in enumerate(header_row):
                #print(index, column_header)
                for row in reader:
                    bdt.append(row[0])
                    fom.append(row[3])
    
    order = np.argsort(bdt)
    xs = np.array(bdt)[order]
    ys = np.array(fom)[order]
    return xs, ys
    return bdt, fom 

dm = "50"

LSTORE = "/lstore/cms/dbastos/Stop4Body/"

dirHT200 = LSTORE + "nTuples17_nanoAOD_v2020-09-17-Cl30-DR40-pt30_test_bdt"+dm+"_HT200_Jet1Pt110/BdtCut/"
dirHT250 = LSTORE + "nTuples17_nanoAOD_v2020-09-17-Cl30-DR40-pt30_test_bdt"+dm+"_HT250_Jet1Pt110/BdtCut/"
dirHT300 = LSTORE + "nTuples17_nanoAOD_v2020-09-17-Cl30-DR40-pt30_test_bdt"+dm+"_HT300_Jet1Pt110/BdtCut/"

if dm == "10":
  dirHT200 = LSTORE + "nTuples17_nanoAOD_v2020-09-17-Cl30-DR40-pt30_test_bdt"+dm+"_HT200/BdtCut/"
  dirHT250 = LSTORE + "nTuples17_nanoAOD_v2020-09-17-Cl30-DR40-pt30_test_bdt"+dm+"_HT250/BdtCut/"
  dirHT300 = LSTORE + "nTuples17_nanoAOD_v2020-09-17-Cl30-DR40-pt30_test_bdt"+dm+"_HT300/BdtCut/"

bdt200, fom200 = fomFromFile(dirHT200)
bdt250, fom250 = fomFromFile(dirHT250)
bdt300, fom300 = fomFromFile(dirHT300)

label=["Signal","Background"]

#Plot Data
fig = plt.figure(dpi = 128, figsize = (10,6))
plt.plot(bdt200, fom200,"r", label="HT200")
plt.plot(bdt250, fom250,"g", label="HT250")
plt.plot(bdt300, fom300,"b", label="HT300")
plt.legend(loc='best')
#Format Plot
plt.title("DM="+dm, fontsize = 24)
plt.grid()
plt.ylim(0, 3.5)
plt.xlabel('BDT >',fontsize = 16)
plt.ylabel("FOM", fontsize = 16)
plt.tick_params(axis = 'both', which = 'major' , labelsize = 16)
plt.show()
plt.savefig("/lstore/cms/dbastos/DM"+dm+"_multipleFom.png")