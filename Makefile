# User-specific settings
PREFIX      = /opt
VAR_PATH    = /var/lib/pako
TMP_PATH    = /tmp/pako

CXX         = c++
CXXFLAGS    = -Wall

LOGO_ENABLE = 1 # 0/1
EASTER_EGG  = 1 # 0/1

# DO NOT TOUCH ANYTHING IF YOU DON'T KNOW WHAT IT'S SUPPOSED TO DO

DEBUG      = ## -DDEBUG ## <- Uncomment to enable debugging
VERSION	   = 0.9-dev_build
CMCXXFLAGS = -std=c++17 -DPREFIX=\"$(PREFIX)/\" -DVAR_PATH=\"$(PREFIX)/$(VAR_PATH)/\" -DTMP_PATH=\"$(TMP_PATH)/\" -DVERSION=\"$(VERSION)\" -DEASTER_EGG=$(EASTER_EGG) $(DEBUG)
CMLIBS     = -larchive -llzma
SOURCES    = src/main.cpp src/install.cpp src/remove.cpp src/list.cpp src/help.cpp
OBJ        = $(SOURCES:.cpp=.o)
TARGET     = pako

all: $(OBJ)
	@echo " LD $^ -> pako"
	@g++ $(CXXFLAGS)  -o pako $^ $(CMLIBS) 

%.o: %.cpp
	@echo " CXX $< -> $@"
	g++ $(CXXFLAGS) $(CMCXXFLAGS) -c $< -o $@

clean:
	@echo "Cleaning up..."
	@rm -f $(OBJ) $(TARGET)
