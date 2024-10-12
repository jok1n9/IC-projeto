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

# Find FFTW
# find_package(FFTW3 REQUIRED)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE 
    sfml-graphics 
    sfml-audio
    gnuplot-iostream
    Boost::iostreams
    Boost::system
    Boost::filesystem
    #FFTW3::FFTW3
)
target_compile_features(main PRIVATE cxx_std_17)