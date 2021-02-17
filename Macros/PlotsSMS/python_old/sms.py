from array import *
import CMS_lumi
#m_stop = "m_{#tilde{t}} [GeV]" 
stop   = "#tilde{t}_{1}" 
lsp    = "#lower[-0.20]{#tilde{#chi}}#lower[-0.1]{#scale[0.85]{^{0}}}#kern[-1.3]{#scale[0.85]{_{1}}}"
#m_stop = "m(%s) [GeV]"%stop
m_stop = "m_{%s} [GeV]"%stop
#chargino = "#lower[-0.2]{#tilde{#chi}}#lower[-0.1]{#scale[0.85]{^{#pm}}}#kern[-1.3]{#scale[0.85]{_{1}}}"
chargino  = "#lower[-0.20]{#tilde{#chi}}#lower[-0.2]{#scale[0.85]{^{#pm}}}#kern[-1.3]{#scale[0.85]{_{1}}}"

class sms():

    def __init__(self, modelname):
        #if modelname.find("T1tttt") != -1: self.T1tttt()
        #if modelname.find("T2tt") != -1: self.T2tt()
        #if modelname.find("T5ttttDM175") != -1: self.T5ttttDM175()
        #if modelname.find("T5tttt") != -1: self.T5tttt()
        #if modelname.find("T1bbbb") != -1: self.T1bbbb()
        #if modelname.find("T1qqqq") != -1: self.T1qqqq()
        #if modelname.find("T5qqqqVV") != -1: self.T5qqqqVV()

        modelattr = getattr( self, modelname, None)
        if not modelattr:
            raise Exception("Given model (%s) was not found as a defined SMS model"%modelname)
        modelattr()

    def _setDefault(self):
        self.modelname = "T2tt"
        # decay chain
        self.label= "pp #rightarrow #tilde{t} #tilde{t}, #tilde{t} #rightarrow t "+lsp;
        self.label2= "";
        # scan range to plot
        self.Xmin = 250.
        self.Xmax = 800.
        self.Ymin = 100.
        self.Ymax = 900.
        self.Zmin = 0.1
        self.Zmax = 100.
        # produce sparticle
        #self.sParticle = "m#kern[0.1]{_{#lower[-0.12]{#tilde{t}}}} [GeV]"
        self.sParticle = m_stop 
        # LSP
        #self.LSP = "m#kern[0.1]{_{"+lsp_s+"}} [GeV]"
        self.LSP = "m({lsp}) [GeV]".format(lsp=lsp)
        # turn off diagonal lines
        self.diagOn = False
        self.boxOn = False

    def _setDeltaM(self):
        self.dmplot    = True
        self.Ymin = 10
        self.Ymax = 107
        self.LSP = "#Deltam( {stop}, {lsp} ) [GeV]".format(stop=stop,lsp=lsp)


    def T2DegStop(self):
        self._setDefault()
        # model name
        self.modelname = "T2DegStop"
        # decay chain
        #self.label= "pp #rightarrow #tilde{t}_{1} #tilde{t}_{1}, #tilde{t}_{1} #rightarrow b f f' #tilde{#chi}^{0}_{1}"
        self.label= "pp #rightarrow {stop} {antistop}, {stop} #rightarrow b f f' {lsp}".format(stop=stop, antistop=stop, lsp=lsp)
        # LSP
        #CMS_lumi.cmsTextSize = 0.6

    def T2bW(self):
        # model name
        self._setDefault()
        self.modelname = "T2bW"
        # decay chain
        #self.label= "pp #rightarrow #tilde{t} #tilde{t}, #tilde{t} #rightarrow #tilde{#chi}^{#pm}_{1} b, #tilde{#chi}^{#pm}_{1} #rightarrow f f' #tilde{#chi}^{0}_{1}"
        self.label= "pp #rightarrow {stop} {antistop}, {stop} #rightarrow {chargino} b, {chargino} #rightarrow f f' {lsp}".format(stop=stop, antistop=stop, chargino=chargino, lsp=lsp)
        self.label2 = ""#"m_{#tilde{#chi}^{#pm}_{1}}=(m_{#tilde{t}}+m_{#tilde{#chi}^{0}_{1}})/2"
        #self.extraTextPos = [ 0.17 , 0.735 ]
        #self.extraTextPos = [ 0.17 , 0.68 ]
        #self.extraText = "m_{#tilde{#chi}^{#pm}_{1}}=(m_{#tilde{t}}+m_{#tilde{#chi}^{0}_{1}})/2"
        #self.extraText = "m_{#tilde{#chi}^{#pm}_{1}}=(m_{#tilde{t}}+m_{#tilde{#chi}^{0}_{1}})/2"
        self.extraText = "m#lower[+1.0]{_{%(chargino)s}}=(m_{%(stop)s}+m#lower[1.0]{_{%(lsp)s}})/2"%{'chargino':chargino, 'stop':stop, 'lsp':lsp}
        #self.extraTextPos = [ 0.6 , 0.8 ]
        self.extraTextPos = [ 0.18 , 0.67 ]
        # LSP
        #self.LSP = "m(#tilde{#chi}^{0}_{1}) [GeV]"
        self.extraTextSize = 0.04
        #CMS_lumi.cmsTextSize = 0.6

    def T2DegStop_dm(self):
        self.T2DegStop()
        self._setDeltaM()
        self.modelname = "T2DegStop_dm"
        self.dmplot    = True
        # LSP
        # diagonal position: mLSP = mgluino - 2mtop 

    def T2bW_dm(self):
        self.T2bW()
        self._setDeltaM()
        self.modelname = "T2bW_dm"
        self.extraTextPos = [ 0.16 , 0.67 ]
        self.extraTextSize = 0.038

    def T2bW_signif(self):
        self.T2bW_dm()
        self.signifPlot=True
        self.analysis="arXiv:9999:99999"
        self.extraTextPos = [ 0.17 , 0.82 ]
        self.Ymax = 90
        self.Zmin = -2
        self.Zmax = 3

    def T2DegStop_signif(self):
        self.T2DegStop_dm()
        self.signifPlot=True
        self.analysis="arXiv:9999:99999"
        self.Ymax = 90
        self.Zmin = -2
        self.Zmax = 3


    def T2DegStop_comb_dm(self):
        self.T2DegStop_dm()
        # LSP
        #self.extraTextPos = [ 0.16 , 0.67 ]
        self.Zmin = 0.01
        self.Zmax = 2
        # diagonal position: mLSP = mgluino - 2mtop 

    def T2bW_comb_dm(self):
        self.T2bW_dm()
        self._setDeltaM()
        self.modelname = "T2bW_dm"
        #self.extraTextPos = [ 0.16 , 0.67 ]
        #self.extraTextSize = 0.038
        self.Zmin = 0.01
        self.Zmax = 2


