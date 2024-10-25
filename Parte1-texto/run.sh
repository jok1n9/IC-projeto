#!/bin/bash

# Check if the user has provided an argument
if [ -z "$1" ]; then
    echo "Usage: $0 path\to\file.txt"
    exit 1
fi

# Set the encoding to UTF-8 (optional, depends on your needs)

# Compile the C++ program
g++ -std=c++11 -Wall -o runtexto texto.cpp

# Check if the compilation was successful
if [ $? -ne 0 ]; then
    echo "Compilation failed!"
    exit 1
fi

# Run the C++ program with the provided input file name
./runtexto "$1"

# Check if the C++ program executed successfully
if [ $? -ne 0 ]; then
    echo "Execution of the C++ program failed!"
    exit 1
fi

# Run the Python script to generate histograms
python3 analyse.py

# Check if the Python script executed successfully
if [ $? -ne 0 ]; then
    echo "Execution of the Python script failed!"
    exit 1
fi

echo "All tasks completed successfully!"
