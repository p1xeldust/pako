# PAKO!

Introducing **Pako!**  
The package manager which will be used in currently nonexistent mobile Linux distribution!  
[![Makefile CI](https://github.com/toastmakingmachine/pako/actions/workflows/makefile.yml/badge.svg?branch=main)](https://github.com/toastmakingmachine/pako/actions/workflows/makefile.yml)  

:warning: STILL IN DEVELOPMENT. DEVBUILDS :warning: 

# Installation:
For this you need a c++ compiler compatible with c++17 and ```make```. 
Debian-like distributions:
```
apt install git g++ make libarchive-dev liblzma-dev
```
Once you've installed your compiler properly, clone this repository:
```
git clone https://github.com/toastmakingmachine/pako.git
```
After this step, go to pako folder:
```
cd pako
```
Finally, use ``make`` to compile package manager!
```
make
```
Either you can link Pako statically by using
```
CXXFLAGS="-static"
```
 
You can use ``-j`` flag to specify the  amount of cores to use while compilation. Either you can use all of them:
```
 make -j{nproc}
```

# Functionality:
:exclamation: Important notice :exclamation:  
Currently only these architectures supported:  
	- [x] x86_64
	- [x] i386
	- [x] aarch64
	- [x] arm
	- [x] ppc
	- [x] ppc64
	- [x] mips
	- [x] mipsel
- [x] Pre-installation and Post-installation script support
- [X]  symlinks support
- [X] Support dependencies (somehow)
- [X] Solve dependencies on package removal


