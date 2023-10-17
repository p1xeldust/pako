# Пользовательские переменные для установки пакетов. На здоровье.
PREFIX      = 
VAR_PATH    =
TMP_PATH    =

LOGO_ENABLE = 1 # 0 / 1
EASTER_EGG  = 0 # 0 / 1

CXX         = g++
CXXFLAGS    = -Ofast #-static # -DDEBUG
LDFLAGS     = -Ofast #-static


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
ifeq ($(TMP_PATH),)
LOGO_ENABLE = 0
endif
ifeq ($(TMP_PATH),)
EASTER_EGG  = 0
endif

VERSION	    = "0.9 dev_build"
CMCXXFLAGS  = -std=c++17 -DPREFIX=\"$(PREFIX)/\" -DVAR_PATH=\"$(PREFIX)/$(VAR_PATH)/\" -DTMP_PATH=\"$(TMP_PATH)/\" -DVERSION=\"$(VERSION)\" -DEASTER_EGG=$(EASTER_EGG) -DLOGO_ENABLE=$(LOGO_ENABLE)
CMLIBS      = -larchive -llzma
SOURCES     = src/main.cpp src/install.cpp src/remove.cpp src/list.cpp src/help.cpp
OBJ         = $(SOURCES:.cpp=.o)


all: $(OBJ)
	@echo "  LD   $^ -> pako"
	@g++ $(CXXFLAGS) -o pako $^ $(CMLIBS)

%.o: %.cpp
	@echo "  CXX  $< -> $@"
	@g++ $(CXXFLAGS) $(CMCXXFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	@rm -f $(OBJ) pako

install: all # Я не изверг, поэтому здесь есть DESTDIR
	install -m 755 pako ${DESTDIR}/usr/bin
