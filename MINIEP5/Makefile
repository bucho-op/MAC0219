# Makefile for miniep5

# Compiler
CXX = g++

# Compiler flags (no warnings)
CXXFLAGS = -pthread

# Source files
SRCS = miniep5.cpp

# Output executable name
TARGET = miniep5

# Default target
all: $(TARGET)

# Build target
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# Clean up build files
clean:
	rm -f $(TARGET)

.PHONY: all clean

