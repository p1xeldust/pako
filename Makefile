# Makefile for Pako
# DESTDIR is supported

# Compilation related stuff
CC          = 
CXX         = 
CXXFLAGS    = -Os -no-pie -pipe
LDFLAGS     = -O3 -no-pie -pipe


# Don't touch anything пожалуйста :P

# Setting default variables if some of them are undefined
ifeq ($(CC),)
CC          = cc
endif
ifeq ($(CXX),)
CXX         = c++
endif
ifneq ($(NOSU),1)
NOSU        = 0
endif

VERSION	    = "0.0.2"
CMCXXFLAGS  = -std=c++17 -DVERSION=\""0.0.2\""
CMLIBS      = -larchive -llzma
CXXSRC      = src/common/config.cpp src/common/copy.cpp src/common/dialog.cpp src/common/output.cpp src/common/tar.cpp src/db/add.cpp src/db/read.cpp src/db/remove.cpp src/package/architecture.cpp src/package/check.cpp src/package/cleanup.cpp src/package/dependencies.cpp src/package/exec.cpp src/package/install.cpp src/package/list.cpp src/package/parse.cpp src/package/remove.cpp src/package/specs.cpp src/package/unpack.cpp src/main.cpp
CSRC	    =

CXXOBJ      = $(CXXSRC:.cpp=.o)
COBJ        = $(CSRC:.c=.o)
BIN         = src/pako
BUILDER     = scripts/pako-builder

all: $(CXXOBJ) $(COBJ)
	@echo "  CXXLD  $(BIN)"
	@$(CXX) $(LDFLAGS) -o $(BIN) $^ $(CMLIBS)

%.o: %.c
	@echo "  CC     $@"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	@echo "  CXX    $@"
	@$(CXX) $(CMCXXFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	@rm -f $(COBJ) $(CXXOBJ) $(BIN)

install: all
	@install -d ${DESTDIR}/usr/bin
	@install -d ${DESTDIR}/etc/pako
	@echo "  INST   $(BIN) ${DESTDIR}/usr/bin"
	@install -m 755 $(BIN) ${DESTDIR}/usr/bin
	@echo "  INST   $(BIN) ${DESTDIR}/usr/bin"
	@install -m 755 $(BUILDER) ${DESTDIR}/usr/bin
	@echo "  INST   conf/pako.conf ${DESTDIR}/etc/pako"
	@install -m 644 conf/pako.conf ${DESTDIR}/etc/pako
