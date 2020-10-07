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

	std::string htmin = "100";
	std::string htmax = "200";

	Int_t nbins = 50;

	std::string sample = "sample_WJetsToLNu_HT" + htmin + "to" + htmax+ "_gen_Wpt";

	TFile W16("/lstore/cms/dbastos/Stop4Body/genWptWeights/genWpt_2016.root","READ")
	TTree* t16 = static_cast<TTree*>(W16.Get(sample.c_str()));
	TH1F *hw16 = new TH1F("hw16","gen_Wpt",nbins,0,std::stoi(htmax));
	t16->Draw("gen_Wpt>>hw16");

	TH1F *hs = new TH1F("hs","2017+2018",nbins,0,std::stoi(htmax));
	TH1F *hwnew = new TH1F("hwnew","gen_Wpt",nbins,0,std::stoi(htmax));

	if(year == 2017){
		TFile Wnew("/lstore/cms/dbastos/Stop4Body/genWptWeights/genWpt_2017.root","READ")
		TTree* tnew = static_cast<TTree*>(Wnew.Get(sample.c_str()));
		tnew->Draw("gen_Wpt>>hwnew");
		//hs->Add(hwnew);
		hwnew->SetLineColor(kRed);
	}
	else if(year == 2018){
		TFile Wnew("/lstore/cms/dbastos/Stop4Body/genWptWeights/genWpt_2018.root","READ")
		TTree* tnew = static_cast<TTree*>(Wnew.Get(sample.c_str()));
		tnew->Draw("gen_Wpt>>hwnew");
		//hs->Add(hwnew);
		hwnew->SetLineColor(kGreen);
	}


	hw16->Scale(1.0/hw16->Integral());
	hw16->Draw("");
	hwnew->Draw("same");
	/*
	hs->Scale(1.0/hs->Integral())
	hs->SetLineColor(kRed)
	hs->Draw("same")
	lg->AddEntry(hw16,"2016","l");
	lg->AddEntry(hs,"2017+2018","l");
	*/
	/*
	hw17->Scale(1.0/hw17->Integral())
	hw17->SetLineColor(kRed);
	hw17->Draw("same")
	hw18->Scale(1.0/hw18->Integral())
	hw18->SetLineColor(kGreen);
	hw18->Draw("same")
	lg->AddEntry(hw16,"2016","l");
	lg->AddEntry(hw17,"2017","l");
	lg->AddEntry(hw18,"2018","l");
	*/
	lg->AddEntry(hw16,"2016","l");
	std::string sYear = (std::to_string(year)).c_str()
	lg->AddEntry(hwnew,sYear,"l");

	lg->Draw();

	c1.cd();
	t2->Draw();
	t2->cd();

	TH1F* wrat = static_cast<TH1F*>(hw16->Clone("wrat"));
	wrat->SetLineColor(kBlack);
	wrat->Divide(hwnew);
	//wrat->Divide(hs);
	wrat->GetYaxis()->SetNdivisions(5);
	wrat->GetYaxis()->SetRangeUser(0.49,1.51);
	wrat->Draw();

	c1.SaveAs(("genW_pt_ratio_HT"+ htmin + "to" + htmax+".png").c_str());
	wrat->SaveAs((std::to_string(year)+"_"+sample+".root").c_str());
}