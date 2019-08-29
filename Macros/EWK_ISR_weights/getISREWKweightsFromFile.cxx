double averageBinInterval(int minX, int maxX, int minY, int maxY, TH2D* zptmass_weights, bool verbose = false){
  TAxis* xAxis = zptmass_weights->GetXaxis();
  TAxis* yAxis = zptmass_weights->GetYaxis();

  //xAxis->SetRangeUser(60,120);
  //yAxis->SetRangeUser(0,500);

  xAxis->SetRangeUser(79,101);
  yAxis->SetRangeUser(0,500);

  int binX;
  int binY;
  int nBins = 0;

  double X = minX;
  double Y = minY;

  double scaleY;
//  double scaleYlow;
//  double scaleYup;
  double a;
  double b;
  double yLowEdge;
  double yUpEdge;

  double wSumX = 0;
  double sumX = 0;
  double weightX;
  double widthX;
  double averageX;

  double wSumY = 0;
  double sumY = 0;
  double weightY;
  double widthY;
  double averageY;

  while (Y < maxY) {
    if(verbose){
      cout << "Y: " << Y << '\n';
    }
    binY = yAxis->FindBin(Y);
    scaleY = 1;
//    scaleYlow = 1;
//    scaleYup = 1;

    yLowEdge = yAxis->GetBinLowEdge(binY);
    widthY = yAxis->GetBinWidth(binY);

    // Y LOW boundary
    if (yLowEdge < minY) {
//      scaleYlow = 1 - (minY - yLowEdge)/widthY;
      a = minY - yLowEdge;
    }
    else{
//      scaleYlow = 1;
      a = 0;
    }

    while (X < maxX) {
      if(verbose){
        cout << "  X: " << X << '\n';
      }
      binX = xAxis->FindBin(X);
      X = xAxis->GetBinUpEdge(binX);

      weightX = zptmass_weights->GetBinContent(binX,binY);

      widthX = xAxis->GetBinWidth(binX);
      wSumX += widthX*weightX;
      sumX += widthX;

      nBins++;
      //zptmass_weights->SetBinContent(binX,binY,zptmass_weights->GetBin(binX,binY));
    }
    averageX = wSumX/sumX;
    if(verbose){
      std::cout << "  averageX: " << averageX << '\n';
    }
    X = minX;

    yUpEdge = yAxis->GetBinUpEdge(binY);
    Y = yUpEdge;

    // Y UP boundary
    if (Y > maxY) {
//      scaleYup = 1 - (yUpEdge - maxY)/widthY;
      b = yUpEdge - maxY;
    }
    else{
//      scaleYup = 1;
      b = 0;
    }
    scaleY = 1-(a+b)/widthY;
    if(verbose){
      std::cout << "scaleY: " << scaleY << '\n';
      std::cout << "widthY: " << widthY << '\n';
    }
    wSumY += scaleY*widthY*averageX;
    sumY += scaleY*widthY;
   }
   averageY = wSumY/sumY;
   if(verbose){
     std::cout << "averageY: " << averageY << '\n';
   }
   if(verbose){
     cout << "nBins: " << nBins << endl;
   }
   zptmass_weights->SetMarkerColor(1);
   zptmass_weights->Draw("colz text");

   return averageY;
}

void printTable(int minX, int maxX, int minY, int maxY, TH2D* zptmass_weights, bool verbose = false){
  double weight = averageBinInterval(minX, maxX, minY, maxY, zptmass_weights, verbose);

  cout << minY << " - " << maxY << " | " << weight << '\n';
}

void getISREWKweightsFromFile(){
  int minX = 80;//80;
  int maxX = 100;//100;

  bool verbose = false;

  TFile *file = TFile::Open("zpt_weights_2017.root");
  TH2D *zptmass_weights = (TH2D*)file->Get("zptmass_weights");

  cout << " ISR pT | Correction " << '\n';
  printTable(minX, maxX, 0, 50, zptmass_weights, verbose);
  printTable(minX, maxX, 50, 100, zptmass_weights, verbose);
  printTable(minX, maxX, 100, 150, zptmass_weights, verbose);
  printTable(minX, maxX, 150, 200, zptmass_weights, verbose);
  printTable(minX, maxX, 200, 300, zptmass_weights, verbose);
  printTable(minX, maxX, 300, 400, zptmass_weights, verbose);
  printTable(minX, maxX, 400, 600, zptmass_weights, verbose);
  printTable(minX, maxX, 600, 1000, zptmass_weights, verbose);

}
