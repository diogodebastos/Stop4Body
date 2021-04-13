# Stop4Body
Repository for the analysis code of the stop four body decay analysis

## Before running Stop4Body
First step is to get the nanoAOD samples at T3-cms: https://github.com/diogodebastos/cmgtools-lite/tree/104X_dev_nano_lip
When this step is finished it's time to process samples locally.

To do so, connect to the **lxplus** machines and start the environment:

```$. initS4B-10_4_0.sh```

Then, you want to look into `run_susyStop4Body_nanoAOD_cfg.py` for the configurations, modules and skims on the nanoAOD samples. When you've choose which samples you want to get, to a test to assure everything is ok and as expected by:

```$nanopy.py test18_WjetsPT_NLO run_susyStop4Body_nanoAOD_cfg.py -N 5000 -o year=2018 -o runWJets```

At this step, is also good to look into how long each event takes to run, you're aiming for the ms mark. Then, you can send jobs to **condor** to get your samples by:

```$nanopy_batch.py -o 18_WJetsPT_NLO run_susyStop4Body_nanoAOD_cfg.py --option year=2018 --option runWJets=True -b 'run_condor_simple.sh -t 480 ./batchScript.sh'```

Now, that jobs are running, you can use `submitFailedJobs.py` to see jobs that have failed or have other errors by:

```$python submitFailedJobs.py -i 18_WJetsPT_NLO/ -t 960```

In case a large amount of jobs have failed, use `submitList.py` to submit failed jobs. At the same time, you want to open another lxplus session that checks every interval for jobs that have finished and their respective output root file and moves them to *EOS*:

```$while true; do date; echo ""; source cpSamplesFromAFStoEOS.sh 16 16_WJetsHT; sleep 1800; done```

TODO: save directly to EOS.

The final step is to move the samples to t3cms by:

```$gfal-copy -rv nanoAOD/Stop4Body18/Autumn18_Prod2020Sep29/ srm://srm01.ncg.ingrid.pt:8444/srm/managerv2?SFN=/cmst3/store/user/dchasque/nanoAOD/Stop4Body18/Autumn18_Prod2020Sep29```

## Setup analysis and compiling

In this repo you find 2 setup scripts `initNanoS4B.sh` and `initNanoS4B-forScram.sh`. The first one to setup and run, the latter for compiling the framework. You should copy these 2 files to your home. This framework runs on **SCRAM_ARCH=slc7_amd64_gcc820** so, if using ncg machines, you should connect to **pauli**. I suggest having 2 shell sessions in one you do `$. initNanoS4B.sh` and run your analysis and in the other one do `$. initNanoS4B-forScram.sh` and just use it to compile like `$scram b`.

## Running the analysis
Now we are ready to start searching for stop 4 body decay!

### Generating json files for signal, getting golden JSON, Pileup, (EWK)ISR and LHE weights

To generate signal json files run

`$python generateJSONfiles.py`

Get Data Pileup distributions

`$. getPUJSONs.sh`

Check `setupJSONs.sh` and `setupPaths.sh` to define the samples, the directories and the `$YEAR` you want to use. Then, for **Pileup weights** and **(EWK)ISR weights** run:

`$./runStep0-preProcessSamples.sh`

When jobs are done:

`$./runStep0-preProcessSamples_Part2.sh`

You should have `preProcessSamples.root` in `$PREPROCESSSAMPLES_DIR`.

For **LHE weights** do:

`$./runStep0-LHEWeight.sh.`

When jobs are done:

`$./runStep0-LHEWeight_Part2.sh`

You should have `lheWeights.root` in `$LHESCALE_DIR`.

## Processing Samples
Now we are ready to process our samples. The script that does so is `processSamples.cc`. You should check this when updating weights, cleaning jets, create analysis object collections and add or remove HLTs. On `buildAndSubmitJobs.py` you choose the json files of the samples you want to process. Check the `Templates` folder for `jsonTemplate.json` and `Step1_JobTemplate.sh` and in the binaries `buildJobs.cc` for extra details in this step. To process your samples, run:

`$./runStep1.sh`

## Data/MC agreement
This is an intermediate step you can run anytime during your analysis. But it's important to do it right after you processed your data and simulation samples. Which is, to plot your simulation distributions of multiple variables and be sure they are in accordance with data. This script is based on `makePlots.cc`. To get your Data/MC (and other plots) run:

`$./StepN_makeDataMC.sh`

## The MVA method - Boosted Decision Trees
After having a good Data/MC agreement it's time to move to the next step of the analysis, the MVA method. First we have to split the samples in test and train:

`$./runStep1_part1.5.sh`

And then, add the signal samples and the background samples to be used to train the BDT:

`$./runStep1_part2.sh`

To train the BDTs use Signal full DM to use the kinematic similarity between all the signal points with the same DM and for background use **TTbar** and **WJets** only. From here, you can build your BDT (or other classification methods) on `trainiMVA.cc`. To start the training do:

`$./runStep3_TrainMVA.sh`

and apply it to your samples by:

`$./runStep4.sh`

After this step, if you want, you can compare the BDT (and other variables) shape distribution between different years. The idea is to compare between the samples of 2016 and if you want, compare with miniAOD 2017 too. To do so, you have to go to the **pMacros** directory and run `root` interactively from there:

`$cd pMacros`

`$root -l`

`$.x ShapeProcVarYears.C(30,false,false,true,"BDT")`

After training and applying the BDTs it's time to determinate the cut to apply on the BDT output in order so maximize the signal background separation. The first thing to be done, is to get the processes yields as a function ƒof the BDT score. To do so, run:

`$./runStep5_Part1.sh`

Then, get the upper limit cross-section as a function of the BDT cut:

`$./runStep5_Part2.sh`

And finally plot r from the ULXS, efficiencies and FOM from the datacards:

`$./runStep5_Part3.sh`

With step5, we have determined the BDT cut and therefore, the SR of our analysis. Now, we can get run our DD prediction methods of the background, namely, **WJets**, **TTbar** and **Fakes**. With this next step, we'll also look into the Validation Regions of our MVA, **VR2** and **VR3**, more details in `getDDEstimate.cc`. To run this step do:

`$./runStep6-GetDD.sh`



`$./runStep7-GetSys.sh`

## Datacards and preparation to get the Limits

`$./runStep8-getFinalDataCards.sh`

`finalYieldsUncTable.py`

## Limits

`runFinalStep_Part1.sh`

`runFinalStep_Part2.sh`
