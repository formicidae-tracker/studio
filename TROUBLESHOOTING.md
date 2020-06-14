# Troubleshooting

## Installation from sources on older Ubuntu: Missing required version for cmake or protobuf

Ubuntu 18.04 lacks of support for C++17 std::filesystem, protobuf(>=
3.3.0) and a recent cmake >= 3.12 that will produce many error while
building from sources. You will need to run the following command

Install a more recent `cmake` from kitware APT repository

```bash
wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | sudo apt-key add -
sudo apt-add-repository 'deb https://apt.kitware.com/ubuntu/ bionic main'
```

Install a more recente protobuf from an Ubuntu PPA:
``` bash
sudo add-apt-repository ppa:maarten-fonville/protobuf

```

Install/updates required packages:

```bash
sudo apt update
sudo apt install cmake libprotobuf-dev protobuf-compiler libboost-dev libboost-filesystem-dev
```

## Installation of R bindings from github fails with `there is no TLS stream available`

This is due to the fact that some internal R package where not built
with SSL support as they did not found libssl-dev when they were first
built. It is a common R issue.

You will need to install `libssl-dev` and reinstall and reload the
package `git2r` to enable https support.
