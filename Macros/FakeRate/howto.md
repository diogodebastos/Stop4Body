# How to calculate the FAKE-RATE

Plot variables for fake-rate method: `variables2017-plotFakesTL.json`

```sh
. StepN_makeDataMC.sh
```

Calculate the eTL ratio:

```sh
cd Macros/FakeRate
getFakeRateEstimate --variables ../variables2017-getRatio-electron.json --cuts ../variables2017-getRatio-electron.json
getFakeRateEstimate --variables ../variables2017-getRatio-muon.json --cuts ../variables2017-getRatio-muon.json
```

Force ratios on the last bins of the muon channel

```sh
python ../forceEtlRatio.py
```

copy `tightToLooseRatios_2017.root` to data folder

```sh
cp tightToLooseRatios_2017.root ../../data/
```
