CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++17 -Iinclude -Itesting -MMD -MP
LDFLAGS = 

# cross platform check
ifeq ($(OS),Windows_NT)
    TARGET_EXT = .exe
    RM = del /Q /S
    MKDIR_P = mkdir
    FIX_PATH = $(subst /,\,$1)
else
    TARGET_EXT =
    RM = rm -rf
    MKDIR_P = mkdir -p
    FIX_PATH = $1
endif

SRC = $(wildcard src/*.cpp)
OBJ = $(patsubst src/%.cpp, build/%.o, $(SRC))
DEPS = $(OBJ:.o=.d) # generated dependency files

TEST_SRC = $(wildcard testing/*.cpp)
TEST_OBJ = $(patsubst src/%.cpp, build/test_obj/src/%.o, $(filter-out src/main.cpp, $(SRC))) \
           $(patsubst testing/%.cpp, build/test_obj/testing/%.o, $(TEST_SRC))
TEST_DEPS = $(TEST_OBJ:.o=.d)

TARGET = cli-chess$(TARGET_EXT)
TEST_TARGET = run_tests$(TARGET_EXT)

.PHONY: all clean sanitize test debug

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $(OBJ) -o $(TARGET) $(LDFLAGS)

build/%.o: src/%.cpp
	@$(MKDIR_P) $(call FIX_PATH,build) 2> /dev/null || true
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	-$(RM) $(call FIX_PATH,build) $(TARGET) $(TEST_TARGET) 2> /dev/null

sanitize: 
	$(MAKE) clean
	$(MAKE) all CXXFLAGS="$(CXXFLAGS) -g -fsanitize=address" LDFLAGS="-fsanitize=address"

debug:
	$(MAKE) clean
	$(MAKE) all CXXFLAGS="$(CXXFLAGS) -g -DMDEBUG"


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
	@$(MKDIR_P) $(call FIX_PATH,build/test_obj/src) 2> /dev/null || true
	$(CXX) $(CXXFLAGS) $(RENDER_FLAG) -g -fsanitize=address -c $< -o $@

build/test_obj/testing/%.o: testing/%.cpp
	@$(MKDIR_P) $(call FIX_PATH,build/test_obj/testing) 2> /dev/null || true
	$(CXX) $(CXXFLAGS) $(RENDER_FLAG) -g -fsanitize=address -c $< -o $@

-include $(DEPS)
-include $(TEST_DEPS)