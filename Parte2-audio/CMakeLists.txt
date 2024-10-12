cmake_minimum_required(VERSION 3.8 FATAL_ERROR)
project(CMakeSFMLProject LANGUAGES CXX)

# Set Python path
set(Python3_ROOT_DIR "C:/Program Files/Python310")
set(Python3_EXECUTABLE "C:/Program Files/Python310/python.exe")

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

add_executable(main main.cpp)
target_link_libraries(main PRIVATE 
    sfml-graphics 
    sfml-audio
    gnuplot-iostream
    Boost::iostreams
    Boost::system
    Boost::filesystem
)
target_compile_features(main PRIVATE cxx_std_17)

if(WIN32)
    add_custom_command(
        TARGET main
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()

# If Boost is installed in a non-standard location, you might need to add:
# set(BOOST_ROOT "/path/to/boost")
# Before the find_package(Boost ...) line