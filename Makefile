CXXFLAGS += -std=c++11 -Wall -Ofast -MMD -MP -Iinclude -Istb
LDLIBS += -lm

ifdef ENABLE_OPENMP
CXXFLAGS += -fopenmp
endif

SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(patsubst src/%.cpp,obj/%.o,$(SOURCES))
DEPENDS = $(patsubst %.o,%.d,$(OBJECTS))

-include $(DEPENDS)  # re-compile when headers change

$(shell mkdir -p obj)

obj/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

main: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $@ $(LDLIBS) $(LDFLAGS)

launch%: main
	./main $(patsubst launch%,%,$@)

format:
	clang-format -i src/*.cpp include/*.hpp

clean:
	rm $(OBJECTS) $(DEPENDS)
