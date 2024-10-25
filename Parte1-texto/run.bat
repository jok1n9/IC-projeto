@echo off

REM Check if the user has provided an argument
if "%~1"=="" (
    echo Usage: %0 path\to\file.txt
    exit /b 1
)

REM Compile the C++ program
g++ -std=c++11 -Wall -o runtexto texto.cpp

REM Check if the compilation was successful
if errorlevel 1 (
    echo Compilation failed!
    exit /b 1
)

REM Run the C++ program with the provided input file name
runtexto %~1

REM Check if the C++ program executed successfully
if errorlevel 1 (
    echo Execution of the C++ program failed!
    exit /b 1
)

REM Run the Python script to generate histograms
python analyse.py

REM Check if the Python script executed successfully
if errorlevel 1 (
    echo Execution of the Python script failed!
    exit /b 1
)

echo All tasks completed successfully!
pause
