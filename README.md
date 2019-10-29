[![Build Status][build-status-shield]][build-status-url]
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
    <a href="https://formicidae-tracker.github.io/studio/api/index.html"><strong>Explore the myrmidon API docs »</strong></a>
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
<!--[![Product Name Screen Shot][product-screenshot]](https://example.com)-->
The Studio is a Graphical User Interface to visualize large Ant tracking dataset. It is used to creates Ant Metadata project files (.myrmidon), that catalogs ant identification with the help of fiducial tags, and other properties such as size and shapes and user metadata and provide through the myrmidon API a C/C++ interface to access tracking data set efficiently.

The latter aims to provide an user friendly API to help researcher focus more on the data analysis and less on C++ code usability.


## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. As this stade of the project no release and binary distribution is yet available.

### Prerequesites

*At this moment, this project was only tested on Ubuntu 18.04 and Debian 10 using the default gcc compiler provided by these distributions*

The main dependencies for this project are:
  * a C++17 compiler (tested with gcc 7.4 and 8.3)
  * std::filesystem support (gcc >= 8.3.0) or boost::filesystem (>=1.60.0)
  * cmake (>=3.11)
  * Eigen 3
  * Qt 5
  * Google Protobuf (>=3.3.0)
  * [FORT hermes](https://github.com/formicidae-tracker/hermes) library (>=0.2.2)
 
 You debian based systems, you can install the packaged dependencies using the following commands
 
 ```bash
 sudo apt install build-essentials cmake libeigen3-dev qt5base-dev libprotobuf-dev protobuf-compiler
 ```
 
*__Note__: On Ubuntu 18.04 the lack of support for C++17 std::filesystem, protobuf(>= 3.3.0) and a recent cmake, you will need to performs the following commands:*

```bash
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'
sudo add-apt-repository ppa:maarten-fonville/protobuf
sudo apt update
sudo apt install cmake libprotobuf-dev protobuf-compiler libboost-dev libboost-filesystem-dev
```

To install the hermes library from sources, please use the following commands:

```bash
git clone https://github.com/formicidae-tracker/hermes.git
cd hermes
git checkout v0.2.2
mkdir -p build
cd build cmake .. 
make
sudo make install
```
### Installing

Download the latest sources and compile them with cmake:

```bash
git clone https://github.com/formicidae-tracker/studio.git
cd studio
mkdir -p build
cd build
cmake ../
make
```








<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[build-status-shield]: https://img.shields.io/travis/com/formicidae-tracker/studio/master?style=flat-square
[build-status-url]: https://travis-ci.com/formicidae-tracker/studio
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
