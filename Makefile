# User variables
PREFIX      = /opt
VAR_PATH    =      # On prefix
TMP_PATH    =      # On prefix

CC          = gcc
CXX         = g++
CFLAGS      = -Ofast
CXXFLAGS    = -O3
LDFLAGS     = -Ofast


# Don't touch anything пожалуйста :P

# Setting default variables if some of them are undefined
ifeq ($(PREFIX),)
PREFIX      = /opt
endif
ifeq ($(VAR_PATH),)
VAR_PATH    = /var/lib/pako
endif
ifeq ($(TMP_PATH),)
TMP_PATH    = /tmp/pako
endif
ifeq ($(DEMO),)
DEMO  = 0
endif
ifeq ($(CC),)
CC          = gcc
endif
ifeq ($(CXX),)
CXX         = g++
endif
ifneq ($(NOSU),1)
NOSU        = 0
endif

VERSION	    = "dev"
CMCXXFLAGS  = -std=c++17
CVARIABLES  = -DPREFIX=\"$(PREFIX)/\" -DVAR_PATH=\"$(PREFIX)/$(VAR_PATH)/\" -DTMP_PATH=\"$(PREFIX)/$(TMP_PATH)/\" -DVERSION=\"$(VERSION)\" -DDEMO=$(DEMO) -DNOSU=$(NOSU)
CMLIBS      = -larchive -llzma -lsqlite3
CXXSOURCES  = src/install.cpp src/list.cpp src/main.cpp src/output/version.cpp src/output/help.cpp src/output/print.cpp src/remove.cpp src/db/init.cpp src/db/add.cpp src/db/read.cpp src/db/remove.cpp src/package/solver.cpp src/package/checks.cpp src/package/unpack.cpp src/package/temp.cpp src/package/read.cpp
CSOURCES    =
CXXOBJ      = $(CXXSOURCES:.cpp=.o)
COBJ        = $(CSOURCES:.c=.o)
BIN         = pako
all: $(COBJ) $(CXXOBJ)
	@echo "  CXXLD  $(BIN)"
	@$(CXX) $(CFLAGS) $(CXXFLAGS) -o $(BIN) $^ $(CMLIBS)

%.o: %.c
	@echo "  CC     $< -> $@"
	@$(CC) $(CXXFLAGS) $(CXXCMFLAGS) $(CVARIABLES) -c $< -o $@

%.o: %.cpp
	@echo "  CXX    $< -> $@"
	@$(CXX) $(CXXFLAGS) $(CXXCMFLAGS) $(CVARIABLES) -c $< -o $@

clean:
	@echo "Cleaning up..."
	@rm -f $(COBJ) $(CXXOBJ) pako

install: all # Я не изверг, поэтому здесь есть DESTDIR
	install -d ${DESTDIR}/usr/bin
	install -m 755 pako ${DESTDIR}/usr/bin
	install -m 755 scripts/pako-builder ${DESTDIR}/usr/bin
