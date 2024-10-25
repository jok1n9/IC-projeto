# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = `pkg-config --cflags opencv4` -std=c++17 -Wall -Wextra

# Linker flags (libraries)
LDFLAGS = `pkg-config --libs opencv4`

# Target executable
TARGET = imagem

# Source files
SRCS = imagem.cpp

# Object files
OBJS = $(SRCS:.cpp=.o)

# Output directory
OUTDIR = output

# Default target
all: $(OUTDIR) $(TARGET)

# Ensure output directory exists
$(OUTDIR):
	mkdir -p $(OUTDIR)

# Build the target
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Compile source files into object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up build files
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
	rm -rf $(OUTDIR)/*
