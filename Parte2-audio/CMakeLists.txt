cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Find Boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS iostreams system filesystem)

# Add gnuplot-iostream
add_library(gnuplot-iostream INTERFACE)
target_include_directories(gnuplot-iostream INTERFACE ${CMAKE_SOURCE_DIR})  # Assuming gnuplot-iostream.h is in the project root

# Set FFTW3 paths
set(FFTW3_DIR "C:/Program Files/FFTW3")
set(FFTW3_INCLUDE_DIR "${FFTW3_DIR}")
set(FFTW3_LIBRARY "${FFTW3_DIR}/libfftw3-3.lib")

# Add include directory
include_directories(${FFTW3_INCLUDE_DIR})

# Add library directory
link_directories(${FFTW3_DIR})

add_executable(main main.cpp)
target_link_libraries(main PRIVATE 
    sfml-graphics 
    sfml-audio
    gnuplot-iostream
    Boost::iostreams
    Boost::system
    Boost::filesystem
    ${FFTW3_LIBRARY}
)
target_compile_features(main PRIVATE cxx_std_17)

# Ensure the DLL is copied to the output directory
add_custom_command(TARGET main POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${FFTW3_DIR}/libfftw3-3.dll"
        $<TARGET_FILE_DIR:main>)
