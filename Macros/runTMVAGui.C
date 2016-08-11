{
  //TMVA::TMVAGui("TMVA.root");
  TMVA::mvaeffs("TMVA_s.root", kTRUE, "sqrt(2*(((S+B)*log(((S+B)*(B+0.04*B*B))/(B*B+(S+B)*0.04*B*B)))-(1/0.04*log(1+(0.04*B*B*S)/(B*(B+0.04*B*B))))))");
  //TMVA::mvaeffs("TMVA_s.root", kTRUE, "log(10)");
}
