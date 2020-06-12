# makeHinvWS
Combine workspace creator for the VBF H(Inv) analysis.

```makeWS_percategory.C``` is used to create a workspace .root file starting from fast_carpetnetr inputs. It is tailored to create separate naming conventions depending on era/category.

```makeWS.C``` represents the original version of the code.

```all_percategory.txt``` represents the currently used datacard

```plot*.py``` are plotting macros used to prepare the TF plots.

```mkres.sh``` script for running the full set of results.

# Instructions:
## Setting up fast_datacard (creating the env - only once)

```
source /afs/cern.ch/user/$U/$USER/miniconda2/etc/profile.d/conda.sh
conda create -n test_datacard_new
conda activate test_datacard_new

git clone ssh://git@gitlab.cern.ch:7999/fast-hep/public/fast-datacard.git
source /cvmfs/sft.cern.ch/lcg/views/LCG_94/x86_64-centos7-gcc8-opt/setup.sh
cd fast-datacard
make install-dev2

cd ..
export PATH=~/.local/bin:$PATH
cd -
```
## Running fast_datacard step (from a clean shell) - example for MTR 2017

```
git clone git@github.com:vukasinmilosevic/makeHinvWS.git
cd makeHinvWS/test_df_MTR_2017_2020v1
. run_fast_datacard.sh
```


## Extracting the limit
Following through the analysis steps (fast_carpenter and fast_datacard), in the same directory as ```fast_datacard``` output (reqires setting up combine):

```
root
root> .L ../makeWS_percategory.C++
root> makeWS_percategory("2017","MTR")
. mkresh.sh
```



