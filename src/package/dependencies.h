#ifndef PAKO_DEPENDENCIES_H
#define PAKO_DEPENDENCIES_H

#include "package.h"

int CheckDeps(Package& package);
int CheckConflicts(Package& package);

#endif