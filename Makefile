# Пользовательские переменные для установки пакетов. На здоровье.
PREFIX      = /data/data/com.termux/files/usr
VAR_PATH    = /data/data/com.termux/files/usr/var
TMP_PATH    = /data/data/com.termux/files/usr/tmp

CC          = gcc
CXX         = g++
CFLAGS      = -Ofast -DDEBUG
CXXFLAGS    = -O3 -DDEBUG
LDFLAGS     = -Ofast


# Не трогайте, если не знаете, что к чему

# Проверка переменных, если не заданы
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

VERSION	    = "devel"
CMCXXFLAGS  = -std=c++17
CVARIABLES  = -DPREFIX=\"$(PREFIX)/\" -DVAR_PATH=\"$(PREFIX)/$(VAR_PATH)/\" -DTMP_PATH=\"$(TMP_PATH)/\" -DVERSION=\"$(VERSION)\" -DDEMO=$(DEMO)
CMLIBS      = -larchive -llzma
CXXSOURCES  = src/main.cpp src/install.cpp src/remove.cpp src/list.cpp
CSOURCES    = src/info/help.c src/info/version.c
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
