echo "
# AVAILABLE COLORS:
# kMagenta
# kBlue
# kOrange
# kRed
#####################################################
#FORMAT: input root histo-name line-color area-color
#####################################################
HISTOGRAM $1 EXPOut
EXPECTED $1 gEXPOut0 gP1SOut0 gM1SOut0 kRed kOrange
#OBSERVED $1 gOBSOut0 gOBSUpOut0 gOBSDownOut0 kBlack kGray
# Preliminary Simulation or leave empty
#PRELIMINARY Preliminary
PRELIMINARY Preliminary
# Lumi in fb
LUMI 35.9
# Beam energy in TeV
ENERGY 13
# Analysis name
#ANALYSIS Single-muon analysis
" > $2


