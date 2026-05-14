CXX = g++
CXXFLAGS = -Wall -std=c++17 -Iinclude -Itesting
LDFLAGS = 

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))

TEST_SRC = $(wildcard testing/*.cpp)

TEST_OBJ = $(patsubst src/%.cpp, build/test_obj/src/%.o, $(filter-out src/main.cpp, $(SRC))) \
           $(patsubst testing/%.cpp, build/test_obj/testing/%.o, $(TEST_SRC))

TARGET = cli-chess
TEST_TARGET = run_tests

.PHONY: all clean sanitize test debug

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

debug:
	$(MAKE) clean
	$(MAKE) all CXXFLAGS="$(CXXFLAGS) -DMDEBUG"

# --- TEST TARGETS ---

RENDER ?= 0

ifeq ($(RENDER),1)
    RENDER_FLAG = -DALSO_RENDER
else
    RENDER_FLAG =
endif

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) $(RENDER_FLAG) -g -fsanitize=address $^ -o $@ -fsanitize=address

build/test_obj/src/%.o: src/%.cpp
	mkdir -p build/test_obj/src
	$(CXX) $(CXXFLAGS) $(RENDER_FLAG) -g -fsanitize=address -c $< -o $@

build/test_obj/testing/%.o: testing/%.cpp
	mkdir -p build/test_obj/testing
	$(CXX) $(CXXFLAGS) $(RENDER_FLAG) -g -fsanitize=address -c $< -o $@