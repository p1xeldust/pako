# Pako
[![Makefile CI](https://github.com/toastmakingmachine/pako/actions/workflows/makefile.yml/badge.svg?branch=main)](https://github.com/toastmakingmachine/pako/actions/workflows/makefile.yml)
A good package manager for embedded systems  
~800 KiB for compressed static binary!
## Installation:
### 1. Install liblzma, libarchive, git, make, g++
For Debian based distributions:  
`apt install git g++ make liblzma-dev libarchive-dev`
  
For Alpine:  
`apk add git g++ make xz-dev libarchive`
  
For Fedora:  
`dnf install git clang make xz-devel libarchive`

### 2. Get sources and compile package manager
Use git and download repo:  
`git clone https://github.com/p1xeldust/pako`

Then move to pako folder:  
`cd pako/`
  
Compile pako with make:  
`make`
  
You can also compile it static:  
`CXXFLAGS="-static" make`
  
Use **-j** flag to specify the amount of cores to use while compilation. Either you can use all of them:  
`make -j${nproc}`
