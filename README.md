# Stop4Body
Repository for the analysis code of the stop four body decay analysis

## Before running Stop4Body
First step is to get the nanoAOD samples at T3-cms: https://github.com/diogodebastos/cmgtools-lite/tree/104X_dev_nano_lip
When this step is finished it's time to process samples locally.

## Setup analysis and compiling

In this repo you find 2 setup scripts `initNanoS4B.sh` and `initNanoS4B-forScram.sh`. The first one to setup and run, the latter for compiling the framework. You should copy these 2 files to your home. This framework runs on **SCRAM_ARCH=slc7_amd64_gcc820** so, if using ncg machines, you should connect to **pauli**. I suggest having 2 shell sessions in one you do `$. initNanoS4B.sh` and run your analysis and in the other one do `$. initNanoS4B-forScram.sh` and just use it to compile like `$scram b`.

## Running the analysis
Now we are ready to start searching for stop 4 body decay!

### Generating json files for signal, getting golden JSON, Pileup, (EWK)ISR and LHE weights

To generate signal json files run

`$python generateJSONfiles.py`

Get Data Pilup distributions

`$. getPUJSONs.sh`

Check `setupJSONs.sh` and `setupPaths.sh` to define the samples, the directories and the `$YEAR` you want to use. Then, for **Pileup weights** and **(EWK)ISR weights** run:

`$./runStep0-preProcessSamples.sh`

When jobs are done:

`$./runStep0-preProcessSamples_Part2.sh`

You should have `preProcessSamples.root` in `$PREPROCESSSAMPLES_DIR`.

For **LHE weights** do:

`$./runStep0-LHEWeight.sh.``

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

`./runStep3_TrainMVA.sh`

and apply it to your samples by:

`./runStep4.sh`
