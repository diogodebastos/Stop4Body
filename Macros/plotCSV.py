import os, csv, operator
from matplotlib import pyplot as plt

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description='Process the command line options')

    parser.add_argument('-i', '--inputDirectory', required=True, help='Name of the input directory')
    #parser.add_argument('-o', '--outputDirectory', required=True, help='Base name of the output directory for each BDT')
    parser.add_argument('-e', '--efficiencyAndFOM', action='store_true', help='Plot efficiency and FOM')
    parser.add_argument('-x', '--xsecUL', action='store_true', help='Plot cross section upper limit')
    parser.add_argument('-a', '--plotAll', action='store_true', help='Plot all')
    parser.add_argument('-p', '--preview', action='store_true', help='Preview plots')

    args = parser.parse_args()

    inputDirectory = args.inputDirectory
    if not (os.path.exists(inputDirectory) and os.path.isdir(inputDirectory)):
        parser.error('The given input directory does not exist or is not a directory')

    baseDirectory = os.path.realpath(os.getcwd())
    inputDirectory = os.path.realpath(inputDirectory)

    BDTs = [
          {'name': '10', 'deltaM': 10, 'cut': 0.31, 'highDeltaM': False},
          {'name': '20', 'deltaM': 20, 'cut': 0.39, 'highDeltaM': False},
          {'name': '30', 'deltaM': 30, 'cut': 0.47, 'highDeltaM': False},
          {'name': '40', 'deltaM': 40, 'cut': 0.48, 'highDeltaM': False},
          {'name': '50', 'deltaM': 50, 'cut': 0.45, 'highDeltaM': False},
          {'name': '60', 'deltaM': 60, 'cut': 0.50, 'highDeltaM': False},
          {'name': '70', 'deltaM': 70, 'cut': 0.46, 'highDeltaM': True},
          {'name': '80', 'deltaM': 80, 'cut': 0.44, 'highDeltaM': True},
          ]

    for bdt in BDTs:
        thisInputDirectory = inputDirectory + "_bdt" + str(bdt["deltaM"])
        outputDirectory = thisInputDirectory + "/BdtCut/"

        files = os.listdir(outputDirectory)

        if args.xsecUL or args.plotAll:
            for file in files:
                if not file[-8:]=="xsUL.csv": continue
                bdt=[]
                r=[]
                with open(outputDirectory+file) as f:
                    reader = csv.reader(f)
                    header_row = next(reader)
                    sortedlist = sorted(reader, key=lambda row: row[0])
                    #print sortedlist

                for i in range(0,len(sortedlist)):
                    bdt+=[sortedlist[i][0]]
                    r+=[sortedlist[i][1]]

                minR=min(r)
                fminR = float(minR)
                bdtCut = bdt[r.index(minR)]
                print bdtCut
                #Plot Data
                fig = plt.figure(dpi = 128, figsize = (10,6))
                plt.plot(bdt,r,"-")
                plt.title(file[:9]+"   Cut: "+bdtCut, fontsize = 24)
                plt.grid()
                plt.ylim(fminR-0.05, fminR+0.5)
                plt.xlabel('BDT >',fontsize = 16)
                plt.ylabel("$\sigma$ [pb]", fontsize = 16)
                plt.tick_params(axis = 'both', which = 'major' , labelsize = 16)
                plt.savefig(outputDirectory+"/"+file[:9]+"_xsecUL.pdf")
                if args.preview: plt.show()

        if args.efficiencyAndFOM or args.plotAll:
            label=["Signal","Background"]
            for file in files:
                if not file[-11:]=="eff_fom.csv": continue
                bdt=[]
                effSig=[]
                effBckg=[]
                fom=[]

                with open(outputDirectory+file) as f:
                    reader = csv.reader(f)
                    header_row = next(reader)
                    for index,column_header in enumerate(header_row):
                        #print(index, column_header)
                        for row in reader:
                            bdt.append(row[0])
                            effSig.append(row[1])
                            effBckg.append(row[2])
                            fom.append(row[3])
                #print bdt, effSig, effBckg, fom

                #Plot Data
                fig = plt.figure(dpi = 128, figsize = (10,6))
                plt.plot(bdt,effSig,".g")
                plt.plot(bdt,effBckg,".r")
                plt.legend(label, loc='best')
                #Format Plot
                plt.title(file[:9], fontsize = 24)
                plt.grid()
                plt.yscale('log')
                #plt.ylim(0, 2)
                plt.xlabel("BDT >",fontsize = 16)
                plt.ylabel("Efficiency", fontsize = 16)
                plt.tick_params(axis = 'both', which = 'major' , labelsize = 16)
                plt.savefig(outputDirectory+"/"+file[:9]+"_effs.pdf")
                if args.preview: plt.show()

                #Plot Data
                fig = plt.figure(dpi = 128, figsize = (10,6))
                plt.plot(bdt,fom,".b")
                #Format Plot
                plt.title(file[:9], fontsize = 24)
                plt.grid()
                #plt.ylim(0, 2)
                plt.xlabel('BDT >',fontsize = 16)
                plt.ylabel("FOM", fontsize = 16)
                plt.tick_params(axis = 'both', which = 'major' , labelsize = 16)
                plt.savefig(outputDirectory+"/"+file[:9]+"_fom.pdf")
                if args.preview: plt.show()
