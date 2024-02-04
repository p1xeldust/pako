#ifndef PAKO_DEPS_H
#define PAKO_DEPS_H

#include "package.h"

Package CheckDependencies(Package& package);
Package CheckConflicts(Package& package);

#endif