void wjetsNLOnormST(){
	TCanvas c1("c1", "", 700, 500);

  std::string inDir = "/lstore/cms/dbastos/Stop4Body/nTuples17_nanoAOD_v2021-05-11/DataMC-signal-stack-watchSt/";
  std::string baseName = "Presel_WJetsEn";

  std::string vSt = "St";
  TFile syncPlotST((inDir+baseName+"_"+vSt+"_syncPlot.root").c_str());
  TH1D *hDataST=(TH1D*)syncPlotST.Get("Data");
  TH1D *hMCsumST=(TH1D*)syncPlotST.Get("mcSum");
	
  TH1D* wrat = static_cast<TH1D*>(hDataST->Clone("wrat"));

  wrat->Divide(hMCsumST);
  c1.SetGridx(true);
  c1.SetGridy(true);

  wrat->Draw();
  wrat->SaveAs((inDir+"weights_"+vSt+".root").c_str());
}