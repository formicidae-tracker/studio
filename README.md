[![Build Status][build-status-shield]][build-status-url]
[![Coverage Status][coverage-status-shield]][coverage-status-url]
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![LGPL License][license-shield]][license-url]


<br />
<p align="center">
  <a href="https://github.com/formicidae-tracker/studio">
    <img src="resources/icons/flaticon.com/eucalyp/qr-code.svg" alt="Logo" width="180" height="180">
  </a>

  <h1 align="center">FORT Studio and Myrmidon API</h1>

  <p align="center">
    Graphical interface and general purpose API to analyze ant tracking data
    <br />
    <a href="https://formicidae-tracker.github.io/studio/docs/latest/api/index.html"><strong>Explore the myrmidon API docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/formicidae-tracker/documentation/wiki">FORT Project Wiki</a>
    ·
    <a href="https://github.com/formicidae-tracker/studio/issues">Report Bug</a>
    ·
    <a href="https://github.com/formicidae-tracker/studio/issues">Request Feature</a>
  </p>
</p>

## About the project
<!--[![Product Name Screen
Shot][product-screenshot]](https://example.com)--> The Studio is a
Graphical User Interface to manage and visualize large Ant tracking
datasets. It is used to create Ant Metadata project files
(.myrmidon). Those files catalog ant identification with the help of
fiducial tags, and other properties such as size and shapes and user
metadata. The myrmidon API a C/C++ provides an interface to access a
tracking data set efficiently. Its goal is to provide a user friendly
API to help researchers focus more on the data analysis and less on
C++ code usability.


## Getting Started

There are two method for installing `myrmidon` and FORT studio: the
preferred one, using debian packages or from sources.

### Installation from Debian packages (Ubuntu 18.04)

For ubuntu 18.04 and 20.04 based distribution, you can use the debian
package repository hosted at https://packages.tuleu.science .

* NOTE: Only bionic and focal based distribution and their derivative
  such as Mint Tina are supported at this moment *

#### 1. Add packages.tuleu.science repository to your sources

*NOTE: this step is only required once, and may produce errors if runned a second time*

Add the repository public key to your apt keyring. *You will be prompted for your password*
```bash
wget -O - https://packages.tuleu.science/pubkey.gpg | sudo apt-key add -
```

Append the reporsitory to your sources. *Please only run this command
once and use a text editor on*
`/etc/apt/sources.list.d/packages.tuleu.science.list` *if you need to
correct any error*

```bash
echo "deb https://packages.tuleu.science/ubuntu bionic main" | sudo tee /etc/apt/sources.list.d/packages.tuleu.science.list
```

#### 2. Install/Update FORT Studio

Simply use the usual commands :

* Installation:
```bash
sudo apt update
sudo apt install fort-studio
```
* Update :
```bash
sudo apt update
sudo apt upgrade
```

#### Available packages:

Here are some of the provided packages you can install on your system:

* `fort-studio` : the FORT Studio graphical interface
* `libfort-myrmidon-dev` : the myrmidon public API development file, required for R bindings.
* `libfort-hermes-cpp-dev` : API to directly read tracking data frame in C++ from the network or from `.hermes`, not recommended for end-user
* `libfort-hermes-dev` : C binding to `libfort-hermes-cpp-dev`

### Alternate method: installation from sources

These instructions will get you a copy of the project up and running
on your local machine for development and testing purposes. As this
stade of the project no release and binary distribution is yet
available.

#### 1. Prerequesites


The main dependencies for this project are:
  * a C++17 compiler (tested with gcc 7.4 and 8.3)
  * std::filesystem support (gcc >= 8.3.0) or boost::filesystem (>=1.60.0)
  * OpenCV 3 or 4
  * cmake (>=3.11)
  * Eigen 3
  * Asio Library
  * Qt 5
  * Google Protobuf (>=3.3.0)

 You debian based systems, you can install the packaged dependencies using the following commands

 ```bash
 sudo apt install build-essentials cmake libeigen3-dev qtbase5-dev libprotobuf-dev protobuf-compiler libasio-dev
 ```


#### 2. Building and compilatiion

Download the latest sources and compile them with cmake:

```bash
git clone https://github.com/formicidae-tracker/studio.git
cd studio
mkdir -p build
cd build
cmake ../
make
```

#### 3. Running the unit tests

Unit tests for the myrmidon API are runned through gtest, which is
automatically downloaded by the cmake build process. A `check` target
that will build the tests and run them is also created.

```bash
make check
```

## Other language bindings

Some bindings are available for other language such as R. These are
not port of the myrmidon API, but wrapper for theses languages.  They
require you to install the myrmidon API with development file locally,
by preference using package manager `libfort-myrmidon-dev` package.

``` bash
sudo apt install libfort-myrmidon-dev
```

### R bindings

R bindings are located in `bindings/R`. There are two options to
install them in your local R installation. Both of them required the
`devtools` R package.

#### Install from github

Simply run in R

``` R
devtools::install_git("https://github.com/formicidae-tracker/studio.git",subdir = "bindings/R/FortMyrmidon")
```

#### From a local git tree

First get the local source
``` bash
git clone https::/github.com/formicidae-tracker/studio.git
cd studio/bindings/R
R
```

Now in R simply use `devtools::install`

``` R
devtools::install()
```

## Troubleshouting

Some common installation issues are documentend under [TROUBLESHOOTING.md](TROUBLESHOOTING.md).


## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests.

## Roadmap

The roadmap is managed using GitHub's [project](https://github.com/formicidae-tracker/studio/projects). The ROADMAP.md file is a convenience file for the survey form.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [tags on this repository](https://github.com/formicidae-tracker/studio/tags).

## Authors

The file [AUTHORS](AUTHORS) lists all copyright holders (physical or moral person) for this repository.

See also the list of [contributors](https://github.com/formicidae-tracker/studio/contributors) who participated in this project.

## License

This project is licensed under the GNU Lesser General Public License v3.0 or later - see the [COPYING.LESSER](COPYING.LESSER) file for details





<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[build-status-shield]: https://img.shields.io/travis/com/formicidae-tracker/studio/master?style=flat-square
[build-status-url]: https://travis-ci.com/formicidae-tracker/studio
[coverage-status-shield]: https://img.shields.io/coveralls/github/formicidae-tracker/studio?style=flat-square
[coverage-status-url]: https://coveralls.io/github/formicidae-tracker/studio
[contributors-shield]: https://img.shields.io/github/contributors/formicidae-tracker/studio.svg?style=flat-square
[contributors-url]: https://github.com/formicidae-tracker/studio/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/formicidae-tracker/studio.svg?style=flat-square
[forks-url]: https://github.com/formicidae-tracker/studio/network/members
[stars-shield]: https://img.shields.io/github/stars/formicidae-tracker/studio.svg?style=flat-square
[stars-url]: https://github.com/formicidae-tracker/studio/stargazers
[issues-shield]: https://img.shields.io/github/issues/formicidae-tracker/studio.svg?style=flat-square
[issues-url]: https://github.com/formicidae-tracker/studio/issues
[license-shield]: https://img.shields.io/github/license/formicidae-tracker/studio.svg?style=flat-square
[license-url]: https://github.com/formicidae-tracker/studio/blob/master/COPYING.LESSER
[product-screenshot]: images/screenshot.png
