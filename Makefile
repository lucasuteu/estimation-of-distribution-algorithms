SHELL := /usr/bin/env bash

CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -I include
LDFLAGS :=

TARGET := build/eda.exe
SRCS := $(wildcard src/*.cpp)
OBJS := $(SRCS:src/%.cpp=build/%.o)

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(OBJS) | build
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

build:
	mkdir -p build

run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build
