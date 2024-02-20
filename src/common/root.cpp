#include <unistd.h>

#include "output.h"

void CheckRoot() {
    #if NOSU == 0
    if(!getuid()) return;
    output.error("Action requires root privileges.");
    exit(127);
    #endif
}