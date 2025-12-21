# Usage:
#   make day=3
#   make run day=3
#   make debug day=3
#   make run-debug day=3

CXX := g++

RELEASE_FLAGS := -O2 -std=c++23
DEBUG_FLAGS   := -O0 -g -std=c++23 -DDEBUG

# Default target: build the chosen day (release)
.PHONY: all
all: build

# Normalize the day number to two digits
DAY := $(shell printf "%02d" $(day))

# Executable names
TARGET        := day$(day)

build:
	$(CXX) $(RELEASE_FLAGS) src/day$(DAY).cpp src/utils.cpp -I include -o $(TARGET)

run: build
	./$(TARGET)

debug:
	$(CXX) $(DEBUG_FLAGS) src/day$(DAY).cpp src/utils.cpp -I include -o $(TARGET)

run-debug: debug
	./$(TARGET)

clean:
	rm -f day{1..25}
