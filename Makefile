CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude
LDFLAGS = 

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))

TARGET = cli-chess

.PHONY: all clean sanitize

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

build/%.o: src/%.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build $(TARGET)

sanitize: 
	$(MAKE) clean
	$(MAKE) all CXXFLAGS="$(CXXFLAGS) -g -fsanitize=address" LDFLAGS="-fsanitize=address"