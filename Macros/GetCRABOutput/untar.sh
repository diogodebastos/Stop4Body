#!/bin/bash

outDir=$1
intar=$2
skim=$3


echo mkdir -p $outDir
mkdir -p $outDir

echo tar xzf $intar -C $outDir --strip-components 1
tar xzf $intar -C $outDir --strip-components 1

if [[ $skim == 1 ]]; then
    root -l -b -q Skimmer.C+\(\"$outDir\"\) &> /dev/null
    mv ${outDir}/treeProducerStop4Body/tree_skimmed.root ${outDir}/treeProducerStop4Body/tree.root
fi
