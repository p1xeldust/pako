# Compilation stuff
CC          = 
CXX         =
CXXFLAGS    = -pipe -s -UNDEBUG -fno-ident -feliminate-unused-debug-types -feliminate-unused-debug-symbols -fno-dwarf2-cfi-asm -fno-unwind-tables -fno-asynchronous-unwind-tables -fno-debug-cpp -fno-debug-types-section -gno-record-gcc-switches -gno-column-info -gno-statement-frontiers -gno-variable-location-views -gno-internal-reset-location-views -gno-inline-points -fno-rtti -gno-pubnames -fsplit-paths -fdce -fdse -fallocation-dce -ftree-builtin-call-dce -ftree-dce -ftree-dse -fno-semantic-interposition -fgraphite-identity -floop-nest-optimize -floop-interchange -fno-plt -Ofast -flto -fipa-pta -fdevirtualize-at-ltrans -fwhole-program -fno-exceptions -static
LDFLAGS     = -Ofast


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
CMCXXFLAGS  = -std=c++17
CMLIBS      = -larchive -llzma
CXXSOURCES  = 
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