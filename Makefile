CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude -Itesting
LDFLAGS = 

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))

TEST_SRC = $(filter-out src/main.cpp, $(SRC)) $(wildcard testing/*.cpp)
TEST_OBJ = $(patsubst %.cpp, build/%.o, $(subst testing/, testing_obj/, $(TEST_SRC))) 

TARGET = cli-chess
TEST_TARGET = run_tests

.PHONY: all clean sanitize test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

build/%.o: src/%.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf build $(TARGET) $(TEST_TARGET)

sanitize: 
	$(MAKE) clean
	$(MAKE) all CXXFLAGS="$(CXXFLAGS) -g -fsanitize=address" LDFLAGS="-fsanitize=address"

test: clean
	$(CXX) $(CXXFLAGS) -g -fsanitize=address $(TEST_SRC) -o $(TEST_TARGET) -fsanitize=address
	./$(TEST_TARGET)