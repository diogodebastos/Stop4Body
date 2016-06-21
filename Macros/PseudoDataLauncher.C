void PseudoDataLauncher(){

gROOT->ProcessLine(".L MapLoader.C+");
gROOT->ProcessLine(".x PseudoData.C");

}
