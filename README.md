[![Build Status][build-status-shield]][build-status-url]
[![Coverage Status][coverage-status-shield]][coverage-status-url]
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![LGPL License][license-shield]][license-url]


<br />
<p align="center">
  <a href="https://github.com/formicidae-tracker/myrmidon">
    <img src="resources/icons/fort-studio.svg" alt="Logo" width="180" height="180">
  </a>

  <h1 align="center">FORT myrmidon and Studio </h1>

  <p align="center">
	Post-processing API and User Interface for the FORmicidae Tracker.
    <br />
    <a href="https://formicidae-tracker.github.io/myrmidon/docs/latest/api/index.html"><strong>`fort-myrmidon` API docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/formicidae-tracker/documentation/wiki">FORT Project Wiki</a>
    ·
    <a href="https://github.com/formicidae-tracker/myrmidon/issues">Report Bug</a>
    ·
    <a href="https://github.com/formicidae-tracker/myrmidon/issues">Request Feature</a>
  </p>
</p>

## About the project
<!-- [![Product Name Screen Shot][product-screenshot]](https://example.com) -->

`fort-myrmidon` is an API to facilitate access and analyzis of manage
and visualize large Ant tracking datasets. It is used to manipulate
metadata files (.myrmidon). Those files catalogs ant identifications
with the help of fiducial tags, and other properties such as size and
shapes and user-defined metadata.

`fort-studio` is a graphical interface that helps manual creation and
editing of `myrmidon` files, and to provide basic visualization of
tracking datasets.

## Getting Started

`fort-myrmidon` and all the supported bindings (R,python) is primarly
distributed with
[conda](https://docs.conda.io/projects/conda/en/latest/).

`fort-studio` is principally distributed using `snap`. However it is
also distributed with `conda` to allow to pinpoint a perticular
release.

### Install `fort-myrmidon` from `conda`

The following packages are available from the anaconda channel
[formicidae-tracker](https://anaconda.org/formicidae-tracker) :

* `libfort-myrmidon`: C++ library and development files
* `r-fort-myrmidon`: R bindings for `libfort-myrmidon`. R 3.6 and 4.0 are currently supported
* [not available yet] `py-fort-myrmidon`: Python bindings for `libfort-myrmidon`
* `fort-studio`: conda distibution of `fort-studio`. It is provided to
  allow pinpointing a perticular version for archiving purpose. ** In
  almost all cases, users should not install this package on their
  system and the `snap` package should be preferred **. Indeed this
  package is provided to be able to install an older, archived version
  to re-perform an experiment. Please remind that care must be taken
  if you install this conda package, as the command line executable
  `fort-studio` may not match anymore the application `FORT Studio`
  available through the `snap` package.

**Important**: These packages requires your conda installation to
source the `conda-forge` channel, or you may most likely run into
dependency conflict errors. Please follow the [official conda-forge
instructions](https://conda-forge.org/docs/user/introduction.html#how-can-i-install-packages-from-conda-forge)
to enable sourcing packages from `conda-forge`.


### Install `fort-studio` from snap

`fort-studio` is available through its own snap. It comes with its own
bundled version of `fort-myrmidon`. It can be installed using the
following command:

``` bash
sudo snap install fort-studio
```


If your tracking data is not in your home but on a removable media,
you must allow `fort-studio` the access to removable media using the
following command:

``` bash
sudo snap connect fort-studio:removable-media :removable-media`

```

### Installation from sources

This project uses CMake as a build system. It requires a minimum version of 3.11 at least as it needs to fetch some external dependencies using `fetch_content`. It also requires the following dependencies:

* OpenCV 3 or 4
* cmake (>=3.11)
* Eigen 3
* Asio Library
* Qt 5
* Google Protobuf (>=3.3.0)
* Google glog
* yaml-cpp
* TBB


## Contributing

Please read [CONTRIBUTING.md](CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests.

## Versioning

We use [SemVer](http://semver.org/) for versioning. For the versions available, see the [releases](https://github.com/formicidae-tracker/myrmidon/releases).

## Authors

The file [AUTHORS](AUTHORS) lists all copyright holders (physical or moral person) for this repository.

See also the list of [contributors](https://github.com/formicidae-tracker/myrmidon/contributors) who participated in this project.

## License

This project is licensed under the GNU Lesser General Public License v3.0 or later - see the [COPYING.LESSER](COPYING.LESSER) file for details



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[build-status-shield]: https://github.com/formicidae-tracker/myrmidon/actions/workflows/run_tests.yml/badge.svg
[build-status-url]: https://github.com/formicidae-tracker/myrmidon/actions/workflows/run_tests.yml
[coverage-status-shield]: https://img.shields.io/coveralls/github/formicidae-tracker/myrmidon
[coverage-status-url]: https://coveralls.io/github/formicidae-tracker/myrmidon
[contributors-shield]: https://img.shields.io/github/contributors/formicidae-tracker/myrmidon.svg
[contributors-url]: https://github.com/formicidae-tracker/myrmidon/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/formicidae-tracker/myrmidon.svg
[forks-url]: https://github.com/formicidae-tracker/myrmidon/network/members
[stars-shield]: https://img.shields.io/github/stars/formicidae-tracker/myrmidon.svg
[stars-url]: https://github.com/formicidae-tracker/myrmidon/stargazers
[issues-shield]: https://img.shields.io/github/issues/formicidae-tracker/myrmidon.svg
[issues-url]: https://github.com/formicidae-tracker/myrmidon/issues
[license-shield]: https://img.shields.io/github/license/formicidae-tracker/myrmidon.svg
[license-url]: https://github.com/formicidae-tracker/myrmidon/blob/master/COPYING.LESSER
