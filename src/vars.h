#include <string>
#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace constants {
//#define DEBUG // Включение отладочных

#define LOGO "\n    ██████████  ███  ███    ████   ███████    ██████   \n   ███      ███ ████  ███  ███   ███     ██      █████ \n   ███     ███ ███ ███ █████     ██       ██      █████\n   █████████  ███   ███ ██████   ██        ██    ████  \n   ████      ██████████ ███  ███ ███        ██         \n    ████    ████     ███ ███  ███ ███      ███  ███    \n    ███    ███        ███ ██  ████ ████████    ██      \n"

#ifdef __x86_64__
#define HOST_ARCH "amd64"
#endif

#ifdef __x86__
#define HOST_ARCH "i386"
#endif

#ifdef __arm__
#define HOST_ARCH "armhf"
#endif

#ifdef __aarch64__
#define HOST_ARCH "aarch64"
#endif

const std::string VERSION  = "0.2-dev_build";
const std::string PREFIX   = "/opt/";
const std::string VAR_PATH = "//var/lib/pako/";
const std::string TMP_PATH = "/tmp/pako/";
}

#endif
