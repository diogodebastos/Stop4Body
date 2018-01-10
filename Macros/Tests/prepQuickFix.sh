#!/bin/bash

OUTPUT=./plotsTest/

# Start by removing the BDT plots corresponding to the preselection
rm ${OUTPUT}/*_BDT*
rm ${OUTPUT}/*_lowDM_*

# Loop for each DM
for deltaM in 10 20 30 40 50 60 70 80; do
  # Skip directories that do not exist
  if [[ ! -d ${OUTPUT}/DeltaM${deltaM} ]] ; then
    continue
  fi

  # Remove all plots not related to BDT output
  find ${OUTPUT}/DeltaM${deltaM} -type f ! -name "*_BDT*" -exec rm {} \;
  #find ${OUTPUT}/DeltaM${deltaM} -type f ! -name "*_BDT*" -exec if [[ {} =~ .*\.(csv|png) ]] ; then echo {}; fi \;
  #find plots/DeltaM10/ -type f ! -name "*_BDT*" -exec [[ ! {} =~ .*\.(tex|txt) ]] \; -exec echo {} \;

  # Remove high/low depending on which dm is being considered
  if [ "$deltaM" -gt "60" ] ; then
    rm ${OUTPUT}/DeltaM${deltaM}/*_lowDM_*
  else
    rm ${OUTPUT}/DeltaM${deltaM}/*_highDM_*
  fi
done
