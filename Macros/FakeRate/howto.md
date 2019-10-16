# How to calculate the FAKE-RATE

Plot variables for fake-rate method: `variables2017-plotFakesTL.json`

```sh
. StepN_makeDataMC.sh
```

Calculate the eTL ratio:

```sh
cd Macros/FakeRate
getFakeRateRatio --variables ../variables2017-getRatio-electron.json --cuts ../variables2017-getRatio-electron.json 1> ./getFakeRateRatio-el.log 2> ./getFakeRateRatio-el.err
getFakeRateRatio --variables ../variables2017-getRatio-muon.json --cuts ../variables2017-getRatio-muon.json 1> ./getFakeRateRatio-mu.log 2> ./getFakeRateRatio-mu.err
```

Force ratios on the last bins of the muon channel

```sh
python ../forceEtlRatio.py
```

copy `tightToLooseRatios_2017.root` to data folder

```sh
cp tightToLooseRatios_2017.root ../../data/
```

Do Monte Carlo Closure

runFakeMCClosure.sh

```sh
. runFakeMCClosure.sh
```
