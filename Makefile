GLFW_VERSION = 3.4

CXXFLAGS += -std=c++11 -Wall -Ofast -MMD -MP
CXXFLAGS += -Istb
LDFLAGS += -L.

ifdef ENABLE_OPENMP
CXXFLAGS += -fopenmp
endif

SOURCES = main.cpp
DEPS =

# imgui
CXXFLAGS += -Iimgui -Iimgui/backends
SOURCES += $(addprefix imgui/, imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp)
SOURCES += $(addprefix imgui/backends/, imgui_impl_glfw.cpp imgui_impl_opengl3.cpp)

# glfw
LDLIBS += -lglfw3
CXXFLAGS += -Iglfw-$(GLFW_VERSION)/include
DEPS += libglfw3.a

# glad
CFLAGS += -Iglad/include
CXXFLAGS += -Iglad/include
SOURCES += glad/src/glad.c

OBJECTS = $(addsuffix .o, $(basename $(SOURCES)))
DEPENDS = $(patsubst %.o, %.d, $(OBJECTS))

-include $(DEPENDS)  # re-compile when headers change

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
LDLIBS += $(addprefix -l,m GL X11 pthread Xrandr Xi dl)
endif

ifeq ($(UNAME_S), Darwin)
LDLIBS += $(addprefix -framework ,OpenGL Cocoa IOKit)
endif

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main: $(DEPS) $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDLIBS) $(LDFLAGS)

# GLFW stuff
glfw-$(GLFW_VERSION).zip:
	wget -nv https://github.com/glfw/glfw/releases/download/$(GLFW_VERSION)/glfw-$(GLFW_VERSION).zip

libglfw3.a: glfw-$(GLFW_VERSION).zip
	-rm -rf glfw-$(GLFW_VERSION)
	unzip -qq glfw-$(GLFW_VERSION).zip
	cd glfw-$(GLFW_VERSION) && cmake -S . -B build && make -C build
	cp glfw-$(GLFW_VERSION)/build/src/libglfw3.a .

format:
	clang-format -i *.cpp *.hpp

clean:
	rm $(OBJECTS) $(DEPENDS)
