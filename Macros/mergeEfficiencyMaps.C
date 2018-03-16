

void mergeEfficiencyMaps()
{
  TStyle tdrStyle("tdrStyle","Style for P-TDR");
  tdrStyle.SetCanvasBorderMode(0);
  tdrStyle.SetCanvasColor(kWhite);
  tdrStyle.SetCanvasDefH(600);
  tdrStyle.SetCanvasDefW(600);
  tdrStyle.SetCanvasDefX(0);
  tdrStyle.SetCanvasDefY(0);
  tdrStyle.SetPadBorderMode(0);
  tdrStyle.SetPadColor(kWhite);
  tdrStyle.SetPadGridX(false);
  tdrStyle.SetPadGridY(false);
  tdrStyle.SetGridColor(0);
  tdrStyle.SetGridStyle(3);
  tdrStyle.SetGridWidth(1);
  tdrStyle.SetFrameBorderMode(0);
  tdrStyle.SetFrameBorderSize(1);
  tdrStyle.SetFrameFillColor(0);
  tdrStyle.SetFrameFillStyle(0);
  tdrStyle.SetFrameLineColor(1);
  tdrStyle.SetFrameLineStyle(1);
  tdrStyle.SetFrameLineWidth(1);

  tdrStyle.SetHistLineColor(1);
  tdrStyle.SetHistLineStyle(0);
  tdrStyle.SetHistLineWidth(1);
  tdrStyle.SetEndErrorSize(2);
  tdrStyle.SetMarkerStyle(20);
  tdrStyle.SetOptFit(1);
  tdrStyle.SetFitFormat("5.4g");
  tdrStyle.SetFuncColor(2);
  tdrStyle.SetFuncStyle(1);
  tdrStyle.SetFuncWidth(1);
  tdrStyle.SetOptDate(0);
  tdrStyle.SetOptFile(0);
  tdrStyle.SetOptStat(0);
  tdrStyle.SetStatColor(kWhite);
  tdrStyle.SetStatFont(42);
  tdrStyle.SetStatFontSize(0.025);
  tdrStyle.SetStatTextColor(1);
  tdrStyle.SetStatFormat("6.4g");
  tdrStyle.SetStatBorderSize(1);
  tdrStyle.SetStatH(0.1);
  tdrStyle.SetStatW(0.15);
  tdrStyle.SetPadTopMargin(0.08);
  tdrStyle.SetPadBottomMargin(0.13);
  tdrStyle.SetPadLeftMargin(0.16);
  tdrStyle.SetPadRightMargin(0.17);

  tdrStyle.SetOptTitle(0);
  tdrStyle.SetTitleFont(42);
  tdrStyle.SetTitleColor(1);
  tdrStyle.SetTitleTextColor(1);
  tdrStyle.SetTitleFillColor(10);
  tdrStyle.SetTitleFontSize(0.05);
  tdrStyle.SetTitleColor(1, "XYZ");
  tdrStyle.SetTitleFont(42, "XYZ");
  tdrStyle.SetTitleSize(0.06, "XYZ");
  tdrStyle.SetTitleXOffset(0.9);
  tdrStyle.SetTitleYOffset(1.25);
  tdrStyle.SetLabelColor(1, "XYZ");
  tdrStyle.SetLabelFont(42, "XYZ");
  tdrStyle.SetLabelOffset(0.007, "XYZ");
  tdrStyle.SetLabelSize(0.05, "XYZ");
  tdrStyle.SetAxisColor(1, "XYZ");
  tdrStyle.SetStripDecimals(true);
  tdrStyle.SetTickLength(0.03, "XYZ");
  tdrStyle.SetNdivisions(510, "XYZ");
  tdrStyle.SetPadTickX(1);
  tdrStyle.SetPadTickY(1);
  tdrStyle.SetOptLogx(0);
  tdrStyle.SetOptLogy(0);
  tdrStyle.SetOptLogz(0);
  tdrStyle.SetPaperSize(20.,20.);
  tdrStyle.SetHatchesLineWidth(5);
  tdrStyle.SetHatchesSpacing(0.05);
  tdrStyle.cd();


  double DMs[] = {10, 20, 30, 40, 50, 60, 70, 80};
  double stopMs[] = {250, 275, 300, 325, 350, 375, 400, 425, 450, 475, 500, 525, 550, 575, 600, 625, 650, 675, 700, 725, 750, 775, 800};

  TFile outputFile("EfficiencyMaps/combined.root", "RECREATE");

  double minMass = 250;
  double maxMass = 800;
  double massStep = 25;
  int massBins = (maxMass-minMass)/massStep + 1;
  double minDM = 10;
  double maxDM = 80;
  double DMStep = 10;
  int dmBins = (maxDM - minDM) / DMStep + 1;
  double stopM = 0;
  double neutM = 0;
  TH2D * effMap = new TH2D("efficiencyMap", "efficiencyMap", massBins, minMass - massStep/2, maxMass + massStep/2, dmBins, minDM - DMStep/2, maxDM + DMStep/2);
  effMap->GetXaxis()->SetTitle("M_{#tilde{t}_{1}} [GeV]");
  effMap->GetYaxis()->SetTitle("M_{#tilde{t}_{1}} - M_{#tilde{#chi}#lower[0.2]{#scale[0.85]{^{0}}}#kern[-1.3]{#lower[0.2]{#scale[0.85]{_{1}}}}} [GeV]");
  effMap->GetZaxis()->SetTitle("Efficiency [\%]");

  double scale = 0.9;
  effMap->GetXaxis()->SetTitleSize(0.05*scale);
  effMap->GetXaxis()->SetLabelSize(0.05*scale);
  effMap->GetXaxis()->SetTitleOffset(1);
  effMap->GetYaxis()->SetTitleSize(0.06*scale);
  effMap->GetYaxis()->SetTitleOffset(1.25);
  effMap->GetYaxis()->SetLabelSize(0.06*0.8*scale);
  effMap->GetYaxis()->SetLabelOffset(0.01);

  for(auto & DM : DMs)
  {
    std::string fileName;
    std::stringstream converter;
    converter << "EfficiencyMaps/map_bdt" << DM << ".root";
    converter >> fileName;

    TFile inputTFile(fileName.c_str(), "READ");
    TH2D* inputHist = static_cast<TH2D*>(inputTFile.Get("efficiencyMap"));
    for(auto & stopM : stopMs)
    {
      double efficiency = inputHist->GetBinContent(inputHist->FindBin(stopM, DM));
      effMap->Fill(stopM, DM, efficiency);
    }
  }

  effMap->Write();
  outputFile.Write();

  TCanvas c1("c1", "c1", 800, 600);
  effMap->Draw("colz");

  double H = c1.GetWh();
  double W = c1.GetWw();
  double l = c1.GetLeftMargin();
  double t = c1.GetTopMargin();
  double r = c1.GetRightMargin();
  double b = c1.GetBottomMargin();
  double e = 0.025;
  char Buffer[1024];
  sprintf(Buffer, "%.1f fb^{-1} (%.1f TeV)", 35.9, 13.0);
  TLatex *   tex  = new TLatex();
  tex->SetNDC();
  tex->SetTextAngle(0);
  tex->SetTextColor(kBlack);
  tex->SetTextFont(42);
  tex->SetTextAlign(31);
  tex->SetTextSize(0.6 * t);
  tex->DrawLatex(1-r,1-t+0.15*t,Buffer);

  TLatex *   tex2  = new TLatex();
  tex2->SetNDC();
  tex2->SetTextAngle(0);
  tex2->SetTextColor(kBlack);
  tex2->SetTextFont(61);
  tex2->SetTextSize(0.7*t);
  tex2->SetTextAlign(13);
  //double posX = 1 + 0.045*(1-l-r);
  //double posX = 1-r + 0.045*(1-l-r);
  double posX = l + 0.045*(1-l-r);
  double posY = 1-t + 0.035*(1-t-b) + 0.03;
  char cmsText[] = "CMS";
  tex2->DrawLatex(posX, posY, cmsText);

  TLatex *   tex3  = new TLatex();
  tex3->SetNDC();
  tex3->SetTextAngle(0);
  tex3->SetTextColor(kBlack);
  tex3->SetTextFont(52);
  tex3->SetTextSize(0.7*t);
  tex3->SetTextAlign(13);
  //double posX = 1 + 0.045*(1-l-r);
  //double posX = 1-r + 0.045*(1-l-r);
  double posX2 = l + 0.045*(1-l-r) + 0.09;
  double posY2 = 1-t + 0.035*(1-t-b) + 0.02;
  char cmsText2[] = "Simulation";
  tex3->DrawLatex(posX2, posY2, cmsText2);

  c1.SaveAs("EfficiencyMaps/combined.pdf");

  outputFile.Close();

  return;
}
