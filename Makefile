# Usage:
#   make day=3
#   make run day=3

CXX := g++
CXXFLAGS := -O2 -std=c++23

# Default target: build the chosen day
.PHONY: all
all: build

# Normalize the day number to two digits
DAY := $(shell printf "%02d" $(day))

# Name of the executable
TARGET := day$(day)

build:
	$(CXX) $(CXXFLAGS) src/day$(DAY).cpp src/utils.cpp -I include -o $(TARGET)

run: build
	./$(TARGET)

# Optional: clean
clean:
	rm -f day{1..25}
