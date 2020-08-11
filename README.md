[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![LGPL License][license-shield]][license-url]


<br />
<p align="center">
  <a href="https://github.com/formicidae-tracker/studio">
    <img src="resources/icons/flaticon.com/freepik/icon.svg" alt="Logo" width="180" height="180">
  </a>

  <h1 align="center">FORT Tag Scanner</h1>

  <p align="center">
    A tool to scan manually tag and save them to a CSV file
    <br />
    <br />
    <a href="https://github.com/formicidae-tracker/documentation/wiki">FORT Project Wiki</a>
    ·
    <a href="https://github.com/formicidae-tracker/tag-scanner/issues">Report Bug</a>
    ·
    <a href="https://github.com/formicidae-tracker/tag-scanner/issues">Request Feature</a>
  </p>
</p>

## About the project
FORT Tag Scanner allow you to scan individual tag and save them to a CSV.


## Getting Started



The preferred method to install tag-scanner is to use `snap`.

### Installation with `snap`

Ubuntu and derivatives comes with snap pre-installed, but for other
distrbutions, you may need to [install it
manually](https://snapcraft.io/docs/installing-snapd) before pursuing.

Once `snap` is ready, simply use:

``` bash
sudo snap install fort-tag-scanner
```

By default snap applications do not have the permissions to access the
webcam of your computer. You need to manually allow this connection:

``` bash
sudo snap connections fort-tag-scanner:camera :camera
```

### Installation from sources

You will need the following dependency before running a simple cmake:
 * Qt 5
 * OpenCV 4
 * FORT Myrmidon API

## Running the Application

The snap installation will have added a desktop menu entry for the
`FORT Tag Scanner`. Alternatively you can start form a terminal the
command `fort-tag-scanner`.

For information on how to use the application, please refer to the [Manual.md](manual.md).

## Authors

The file [AUTHORS](AUTHORS) lists all copyright holders (physical or moral person) for this repository.

See also the list of [contributors](https://github.com/formicidae-tracker/studio/contributors) who participated in this project.

## License

This project is licensed under the GNU Lesser General Public License v3.0 or later - see the [COPYING.LESSER](COPYING.LESSER) file for details

<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/formicidae-tracker/tag-scanner.svg?style=flat-square
[contributors-url]: https://github.com/formicidae-tracker/tag-scanner/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/formicidae-tracker/tag-scanner.svg?style=flat-square
[forks-url]: https://github.com/formicidae-tracker/tag-scanner/network/members
[stars-shield]: https://img.shields.io/github/stars/formicidae-tracker/tag-scanner.svg?style=flat-square
[stars-url]: https://github.com/formicidae-tracker/tag-scanner/stargazers
[issues-shield]: https://img.shields.io/github/issues/formicidae-tracker/tag-scanner.svg?style=flat-square
[issues-url]: https://github.com/formicidae-tracker/tag-scanner/issues
[license-shield]: https://img.shields.io/github/license/formicidae-tracker/tag-scanner.svg?style=flat-square
[license-url]: https://github.com/formicidae-tracker/tag-scanner/blob/master/COPYING.LESSER
[product-screenshot]: images/screenshot.png
