#ifndef ARCHITECTURES_H
#define ARCHITECTURES_H 1

// x86
#ifdef __x86_64
        #define HOSTARCH "amd64"
#endif
#ifdef __x86
        #define HOSTARCH "i386"
#endif

// powerpc (ppc)
#ifdef __powerpc__
        #define HOSTARCH "ppc"
#endif
#ifdef __powerpc64__
        #define HOSTARCH "ppc64"
#endif

// mips
#ifdef __mips__
        #define HOSTARCH "mips"
#endif
#ifdef __mipsel__
        #define HOSTARCH "mipsel"
#endif

// arm
#ifdef __arm__
        #define HOSTARCH "arm"
#endif
#ifdef __aarch64__
        #define HOSTARCH "arm64"
#endif

#endif
