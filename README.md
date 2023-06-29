# PAKO!

Introducing **Pako!**
The package manager which will be used in currently nonexistent mobile Linux distribution!

:warning: STILL IN DEVELOPMENT. UNSTABLE :warning: 

Be extremely careful while using pako now: many functions aren't implemented yet and there're might be some bugs.

# Installation:
For this you need a c++ compiler compatible with c++17 and ```make```. 
Debian-like distributions:
```
apt install git g++ make libarchive-dev
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
 
 You can use ``-j`` flag to specify the  amount of cores to use while compilation. Either you can use all of them:
```
 make -j{nproc}
```

# Functions:

### [*] INSTALL
- [x] Unpack to /tmp
- [x] Insert installed package to system list
- [x] Check Name
- [x] Check Version
- [x] Check Architecture
- [x] Compare Package arch with host arch
	 :exclamation: Important notice :exclamation: 
	 Currently only these architectures supported:
	- [x] x86_64
	- [x] i386 (currently unsure it will be supported)
	- [ ] aarch64 (not tested yet)
	- [x] armhf
- [x] Copy files from package
- [x] Pre-installation and Post-installation script
- [X] Support symlinks (FINALLY)
- [x] Clear /tmp after all
### [*] LIST
- [x] List all packages
- [x] List specified packages
### [*] REMOVE
- [x] Check if safe to remove file (not conflicting with other packages)
- [x] Delete files and such
- [ ] Execute Pre-Removal script
### [*] HELP
 - [x] Implemented. Like for me, a beautiful one!

### Important notice for Underlevel and [quantumde](https://github.com/quantumde)

I do strongly prohibit the use, share and modify source code of Pako! package manager for Underlevel and its collaborators, variations and forks. Quantude is prohibited from using, distributing and editing the source code of Pako! I don't feel sorry for him and them. The reason is the risk of licensing violation and infringement of intellectual property rights.


