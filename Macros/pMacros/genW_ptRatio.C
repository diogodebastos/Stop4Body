#include "string"

void genW_ptRatio(){
  TCanvas c1("c1", "", 1200, 1350);

  double ratioPadFraction = 0.20;

  TPad* t1 = new TPad("t1","t1", 0.0, ratioPadFraction, 1.0, 1.0);
  TPad* t2 = new TPad("t2","t2", 0.0, 0.0, 1.0, ratioPadFraction);

  t1->Draw();
  t1->cd();

  gStyle->SetOptStat(000000);
  gStyle->SetPalette(1);

  TLegend *lg = new TLegend(.85,.85,0.99,0.99);
  lg->SetFillColor(0);

  Int_t year = 2018;
  std::string sYear = std::to_string(year);

  std::string htmin = "100";
  std::string htmax = "200";

  Int_t nbins = 50;

  std::string sample = "sample_WJetsToLNu_HT" + htmin + "to" + htmax+ "_gen_Wpt";

  TFile W16("/lstore/cms/dbastos/Stop4Body/genWptWeights/genWpt_2016.root","READ");
  TTree* t16 = static_cast<TTree*>(W16.Get(sample.c_str()));
  TH1F *hw16 = new TH1F("hw16","gen_Wpt",nbins,0,std::stoi(htmax));
  t16->Draw("gen_Wpt>>hw16");
  
  //TH1F *hs = new TH1F("hs","2017+2018",nbins,0,std::stoi(htmax));
  std::string file = "/lstore/cms/dbastos/Stop4Body/genWptWeights/genWpt_"+sYear+".root";
  TFile Wnew(file.c_str(),"READ");
  TTree* tnew = static_cast<TTree*>(Wnew.Get(sample.c_str()));
  hwYear = new TH1F("hwYear",sYear.c_str(),nbins,0,std::stoi(htmax));
  tnew->Draw("gen_Wpt>>hwYear");
  hwYear->SetLineColor(kGreen);

  hw16->Scale(1.0/hw16->Integral());
  hw16->Draw("");
  hwYear->Scale(1.0/hwYear->Integral());
  hwYear->Draw("same");
  lg->AddEntry(hw16,"2016","l");
  lg->AddEntry(hwYear,sYear.c_str(),"l");

  lg->Draw();

  c1.cd();
  t2->Draw();
  t2->cd();
  TH1F* wrat = static_cast<TH1F*>(hw16->Clone("wrat"));
  wrat->SetLineColor(kBlack);
  wrat->Divide(hwYear);
  //wrat->Divide(hs);
  //wrat->GetYaxis()->SetNdivisions(5);
  wrat->GetYaxis()->SetRangeUser(0.79,1.21);
  t2->SetGridx(true);
  t2->SetGridy(true);
  wrat->Draw();

  c1.SaveAs(("genW_pt_ratio_HT"+ htmin + "to" + htmax+".png").c_str());
  wrat->SaveAs(("/lstore/cms/dbastos/Stop4Body/genWptWeights/"+std::to_string(year)+"_"+sample+".root").c_str());
  wrat->SaveAs(("/lstore/cms/dbastos/REPOS/Stop4Body-nanoAOD/CMSSW_8_0_14/src/UserCode/Stop4Body-nanoAOD/data/"+std::to_string(year)+"_"+sample+".root").c_str());
}
