#!/bin/bash

processSamples --json samples.json --outDir ~/local-area/Stop4Body/New/
hadd ~/local-area/Stop4Body/New/Background_bdt.root ~/local-area/Stop4Body/New/Wjets_100to200_bdt.root ~/local-area/Stop4Body/New/Wjets_200to400_bdt.root ~/local-area/Stop4Body/New/Wjets_400to600_bdt.root ~/local-area/Stop4Body/New/Wjets_600toInf_bdt.root ~/local-area/Stop4Body/New/TTJets_LO_bdt.root
