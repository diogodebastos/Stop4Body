

void mergeEfficiencyMaps()
{
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
  outputFile.Close();

  return;
}
