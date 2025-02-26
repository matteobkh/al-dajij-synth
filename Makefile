# MAKEFILE FOR MACOS ONLY #

CXX = g++

# Name of the executable
EXEC = audioAppGuiTest

# Directories
SRC_DIR = ./src
PA_DIR = ./libs/portaudio
IMGUI_DIR = ./libs/imgui

# Source files
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/audio.cpp $(SRC_DIR)/gui.cpp 
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp \
	$(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp \
	$(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_sdl2.cpp \
	$(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

# Objects, compiles .o files first
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
#OBJS = $(SOURCES:%.cpp=%.o)

# Build flags, includes, links
CXXFLAGS = -std=c++11 -I$(IMGUI_DIR) -I$(IMGUI_DIR)/backends
CXXFLAGS += -I$(PA_DIR)/include 
CXXFLAGS += -I./include
CXXFLAGS += -g -Wall -Wformat -pthread

LIBS = -framework OpenGL -framework Cocoa -framework IOKit \
	-framework CoreVideo -framework CoreAudio -framework AudioToolbox \
	-framework AudioUnit -framework CoreServices \
	$(shell sdl2-config --libs) \
	$(PA_DIR)/lib/.libs/libportaudio.a

CXXFLAGS += `sdl2-config --cflags`
CXXFLAGS += -I/usr/local/include -I/opt/local/include

## BUILD RULES

%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(EXEC): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

install-deps: install-portaudio install-imgui
.PHONY: install-deps

uninstall-deps: uninstall-portaudio uninstall-imgui
.PHONY: uninstall-deps

install-portaudio:
	mkdir -p libs
	cd libs && git clone https://github.com/PortAudio/portaudio
	cd $(PA_DIR) && ./configure && $(MAKE) -j
.PHONY: install-portaudio

uninstall-portaudio:
	cd $(PA_DIR) && $(MAKE) uninstall
	rm -rf $(PA_DIR)
.PHONY: uninstall-portaudio

install-imgui:
	mkdir -p libs
	cd libs && git clone https://github.com/ocornut/imgui
.PHONY: install-imgui

uninstall-imgui:
	rm -rf $(IMGUI_DIR)
.PHONY: uninstall-imgui

clean:
	rm -f $(OBJS) imgui.ini
.PHONY: clean
