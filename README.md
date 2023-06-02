# PAKO!

Introducing **Pako!**
The package manager which will be used in currently nonexistent mobile Linux distribution!

:warning: STILL IN DEVELOPMENT. UNSTABLE :warning: 

Be extremely careful while using pako now: many functions aren't implemented yet and there're might be some bugs.

# Installation:

For this you need a c++ compiler compatible with c++17 and ```make```.  
Debian-like distributions:
```
apt install git g++ make 
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

## INSTALL
- [x] Unpack to /tmp
- [x] Insert installed package to system list
- [x] Check package's name
- [x] Check package's version
- [x] Check package's architecture
- [x] Compare package's arch with host's arch  
	 :exclamation: Important notice :exclamation:  
	 Currently only these architectures supported:
	- [x] x86_64 (amd64)
	- [ ] i386 (currently unsure it will be supported)
	- [ ] aarch64 (not tested yet)
	- [x] armhf (not tested yet, but im sure it works well)
- [x] Copy files from package
- [ ] Support symlinks (not implemented yet)
- [x] Clear /tmp after all
## LIST
- [ ] Not implemented yet
## REMOVE
 - [ ] Not implemented yet
## HELP
 - [x] Implemented. Like for me, a beautiful one!
