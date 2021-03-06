import ROOT as rt
from array import *
from sms import *
from smsPlotABS import *
import math

# class producing the 2D plot with xsec colors
class smsPlotXSEC(smsPlotABS):

    def __init__(self, modelname, histo, obsLimits, expLimits, energy, lumi, preliminary, label):
        self.LABEL = label
        self.standardDef(modelname, histo, obsLimits, expLimits, energy, lumi, preliminary)
        # canvas for the plot
        self.c = rt.TCanvas("cCONT_%s" %label,"cCONT_%s" %label,600,600)
        self.histo = histo['histogram']
        # canvas style
        self.setStyle()
        self.setStyleCOLZ()

    # define the plot canvas
    def setStyleCOLZ(self):        
        # set z axis
        self.histo.GetZaxis().SetLabelFont(42)
        self.histo.GetZaxis().SetTitleFont(42)
        self.histo.GetZaxis().SetLabelSize(0.035)
        self.histo.GetZaxis().SetTitleSize(0.035)
        self.histo.SetMinimum(self.model.Zmin)
        self.histo.SetMaximum(self.model.Zmax)

        # define the palette for z axis
        NRGBs = 5
        NCont = 255
        stops = array("d",[0.00, 0.34, 0.61, 0.84, 1.00])
        red= array("d",[0.50, 0.50, 1.00, 1.00, 1.00])
        green = array("d",[ 0.50, 1.00, 1.00, 0.60, 0.50])
        blue = array("d",[1.00, 1.00, 0.50, 0.40, 0.50])
        rt.TColor.CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont)
        rt.gStyle.SetNumberContours(NCont)
        
        self.c.cd()
        self.histo.Draw("colz")
        
        rt.gPad.Update()
        palette = self.histo.GetListOfFunctions().FindObject("palette")
        palette.SetX1NDC(1.-0.18)
        palette.SetY1NDC(0.14)
        palette.SetX2NDC(1.-0.13)
        palette.SetY2NDC(1.-0.08)
        palette.SetLabelFont(42)
        palette.SetLabelSize(0.035)

    def DrawPaletteLabel(self):
        textCOLZ = rt.TLatex(0.98,0.15,"95% CL upper limit on cross section [pb]")
        textCOLZ.SetNDC()
        #textCOLZ.SetTextAlign(13)
        textCOLZ.SetTextFont(42)
        textCOLZ.SetTextSize(0.045)
        textCOLZ.SetTextAngle(90)
        textCOLZ.Draw()
        self.c.textCOLZ = textCOLZ

    def DrawParallelogram(self):
        palette = self.histo.GetListOfFunctions().FindObject("palette")
        self.boxes = [ ]
        dx = self.histo.GetXaxis().GetBinWidth(1)
        dy = self.histo.GetYaxis().GetBinWidth(1)
        xmin = self.emptyHisto.GetXaxis().GetXmin()
        xmax = self.emptyHisto.GetXaxis().GetXmax()
        ymin = self.emptyHisto.GetYaxis().GetXmin()
        ymax = self.emptyHisto.GetYaxis().GetXmax()
        for ix in range(self.histo.GetNbinsX()):
            for iy in range(self.histo.GetNbinsY()):
                v = self.histo.GetBinContent(ix+1,iy+1)
                if v>0.:
                    x = self.histo.GetXaxis().GetBinCenter(ix+1)
                    xl = max(x-dx/2.,xmin)
                    xh = min(x+dx/2.,xmax)
                    y = self.histo.GetYaxis().GetBinCenter(iy+1)
                    yll = max(y-(x-xl)-dy/2.,xl-80.)
                    ylh = min(y-(x-xl)+dy/2.,xl-10.)
                    yhl = max(y+(xh-x)-dy/2.,xh-80.)
                    yhh = min(y+(xh-x)+dy/2.,xh-10.)
                    box = rt.TPolyLine()
                    box.SetNextPoint(xl,yll)
                    box.SetNextPoint(xh,yhl)
                    box.SetNextPoint(xh,yhh)
                    box.SetNextPoint(xl,ylh)
                    box.SetFillStyle(1001)
                    box.SetLineWidth(0)
                    box.SetFillColor(palette.GetValueColor(math.log10(v)))
                    box.Draw("F")
                    self.boxes.append(box)
                    self.histo.SetBinContent(ix+1,iy+1,0.)


            
    def Draw(self):
        self.emptyHisto.GetXaxis().SetRangeUser(self.model.Xmin, self.model.Xmax)
        self.emptyHisto.GetYaxis().SetRangeUser(self.model.Ymin, self.model.Ymax)
        self.emptyHisto.GetZaxis().SetRangeUser(self.model.Zmin, self.model.Zmax)
        self.emptyHisto.Draw()
        self.histo.Draw("COLZsame")
        if not getattr(self.model, 'dmplot' , False):
            self.DrawParallelogram()
        self.DrawLines()
        if getattr(self.model, 'boxOn', False):
            self.DrawCorridor()
        if getattr(self.model, 'diagOn', False):
            self.DrawDiagonal()
        self.DrawText()
        if not getattr(self.model, 'signifPlot', False):
            self.DrawLegend()
        self.DrawPaletteLabel()
        
