# Guide: removing legacy debian packages

Since version v0.7.2, `fort-myrmidon` and `fort-studio` does not
uses debian packages listed on `packages.tuleu.science`
anymore. Instead they are using
[conda](https://docs.conda.io/en/latest) ( for `libfort-myrmidon` ) and
[snap](https://snapcraft.io) ( for `fort-studio`). In order to move to
this new distribution, old debian packages and APT listing of
`packages.tuleu.science` should be removed.

## 1. Remove legacy packages from the system

You should use apt to remove the packages:

 ``` bash
 sudo apt purge libfort-* fort-studio
 ```

## 2. Edit your apt source file to remove

This step is important because if `packages.tuleu.science` would
becomes down, it will prevent your system to receive futures updates
from Ubuntu.

``` bash
sudo rm -f /etc/apt/sources.list.d/package.tuleu.science.list
```

Please also check the content of the file `/etc/apt/sources.list` and
remove any line refering to `packages.tuleu.science`

## 3. Install the wanted package

Refer to the documentation to install the wanted packages.

## Note for R user

If you want to use `fort-myrmidon` with R, you will most certainly be
interested to use the `conda` package `r-fort-myrmidon`, which would
require the use of R packaged with `conda` instead from the one
shipped by your linux distribution or from CRAN. Please refer to the
following source to learn how to use `conda` environment for R
packages:


* https://docs.anaconda.com/anaconda/user-guide/tasks/using-r-language/
* https://docs.anaconda.com/anaconda/navigator/tutorials/create-r-environment/

At the moment, `r-fort-myrmidon` is compiled for R 3.6 and 4.0.

Please note that for `fort-studio` it is advised not to install the
`conda` package `fort-studio` but prefer the `snap` package
`fort-studio`.
