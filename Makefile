# User variables
PREFIX      = /opt
VAR_PATH    = /var/lib/pako     # On prefix
TMP_PATH    = /tmp

CC          =
CXX         =
CFLAGS      = -Ofast -DDEBUG
CXXFLAGS    = -O3 -DDEBUG
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
CVARIABLES  = -DPREFIX="\"$(PREFIX)/\"" -DVAR_PATH="\"$(PREFIX)/$(VAR_PATH)/\"" -DTMP_PATH="\"$(TMP_PATH)/\"" -DVERSION=\"$(VERSION)\" -DDEMO=$(DEMO) -DNOSU=$(NOSU)
CMLIBS      = -larchive -llzma -lsqlite3
CXXSOURCES  = src/package/install.cpp src/package/unpack.cpp src/essential/tar.cpp src/essential/o.cpp src/package/parse.cpp src/package/clean.cpp src/package/specs.cpp src/package/arch.cpp src/package/dependencies.cpp src/db/read.cpp src/essential/dialog.cpp src/db/init.cpp src/package/copyByList.cpp src/db/add.cpp src/package/remove.cpp src/db/remove.cpp src/package/list.cpp src/main.cpp
CSOURCES    =
CXXOBJ      = $(CXXSOURCES:.cpp=.o)
COBJ        = $(CSOURCES:.c=.o)
BIN         = pako
all: $(COBJ) $(CXXOBJ)
	@echo "  CXXLD  $(BIN)"
	@$(CXX) $(CXXFLAGS) $(CXXFLAGS) -o $(BIN) $^ $(CMLIBS)

%.o: %.c
	@echo "  CC     $< -> $@"
	@$(CC) $(CFLAGS) $(CVARIABLES) -c $< -o $@

%.o: %.cpp
	@echo "  CXX    $< -> $@"
	@$(CXX) $(CXXFLAGS) $(CXXCMFLAGS) $(CVARIABLES) -c $< -o $@

clean:
	@echo "Cleaning up..."
	@rm -f $(COBJ) $(CXXOBJ) $(BIN)

install: all # Я не изверг, поэтому здесь есть DESTDIR
	install -d ${DESTDIR}/usr/bin
	install -m 755 $(BIN) ${DESTDIR}/usr/bin
	install -m 755 scripts/pako-builder ${DESTDIR}/usr/bin
