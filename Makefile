CXXFLAGS += -std=c++11 -Wall -Ofast -MMD -MP -Iinclude -Istb -Iimgui -Iimgui/backends
LDLIBS += -lm

ifdef ENABLE_OPENMP
CXXFLAGS += -fopenmp
endif

SOURCES = main.cpp

# imgui stuff
SOURCES += $(addprefix imgui/, imgui.cpp imgui_demo.cpp imgui_draw.cpp imgui_tables.cpp imgui_widgets.cpp)
SOURCES += $(addprefix imgui/backends/, imgui_impl_glfw.cpp imgui_impl_opengl3.cpp)

OBJECTS = $(patsubst %.cpp, obj/%.o, $(notdir $(SOURCES)))
DEPENDS = $(patsubst %.o, %.d, $(OBJECTS))

-include $(DEPENDS)  # re-compile when headers change

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
	LDLIBS += -lGL `pkg-config --static --libs glfw3`
	CXXFLAGS += `pkg-config --cflags glfw3`
endif

ifeq ($(UNAME_S), Darwin)
	LDLIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LDLIBS += -L/usr/local/lib -L/opt/local/lib -L/opt/homebrew/lib
	LDLIBS += -lglfw
	CXXFLAGS += -I/usr/local/include -I/opt/local/include -I/opt/homebrew/include
endif

$(shell mkdir -p obj)

obj/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

obj/%.o: imgui/backends/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDLIBS) $(LDFLAGS)

format:
	clang-format -i *.cpp include/*.hpp

clean:
	rm $(OBJECTS) $(DEPENDS)
