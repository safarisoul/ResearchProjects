CPP_FILES = $(wildcard *.cpp)
OBJ_FILES = $(addprefix obj/,$(notdir $(CPP_FILES:.cpp=.o)))

RepTopk: $(OBJ_FILES)
	g++ -O3 -o $@ $^

obj/%.o: %.cpp
	g++ -c -O3 -o $@ $<
