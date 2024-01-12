# PAKO!

Introducing **Pako!**  
Another package manager!  
[![Makefile CI](https://github.com/toastmakingmachine/pako/actions/workflows/makefile.yml/badge.svg?branch=main)](https://github.com/toastmakingmachine/pako/actions/workflows/makefile.yml)  

:warning: STILL IN DEVELOPMENT :warning:

## Installation:
For this you need **a c++ compiler compatible with c++17**, **libarchive**, **liblzma**, **libsqlite3** and **make**

Debian-like distributions:
```
apt install git g++ make libarchive-dev liblzma-dev libsqlite3-dev
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

