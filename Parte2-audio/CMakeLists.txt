cmake_minimum_required(VERSION 3.28)
project(CMakeSFMLProject LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

# Add options for each optional component
option(USE_PLOTTING "Enable plotting functionality (requires Boost and gnuplot)" OFF)
option(USE_FFT "Enable FFT functionality (requires FFTW3)" OFF)

include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.x
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

# Initialize variables for optional libraries
set(OPTIONAL_LIBS "")
set(OPTIONAL_INCLUDES "")
set(COMPILE_DEFINITIONS "")

if(USE_PLOTTING)
    # Find Boost only if plotting is enabled
    set(Boost_USE_STATIC_LIBS OFF)
    set(Boost_USE_MULTITHREADED ON)
    set(Boost_USE_STATIC_RUNTIME OFF)
    find_package(Boost REQUIRED COMPONENTS iostreams system filesystem)
    
    # Add gnuplot-iostream
    add_library(gnuplot-iostream INTERFACE)
    target_include_directories(gnuplot-iostream INTERFACE ${CMAKE_SOURCE_DIR})
    
    # Add to optional components
    list(APPEND OPTIONAL_LIBS 
        gnuplot-iostream
        Boost::iostreams
        Boost::system
        Boost::filesystem
    )
    list(APPEND COMPILE_DEFINITIONS "USE_PLOTTING")
endif()

if(USE_FFT)
    # Set FFTW3 paths
    set(FFTW3_DIR "C:/Program Files/FFTW3")
    set(FFTW3_INCLUDE_DIR "${FFTW3_DIR}")
    set(FFTW3_LIBRARY "${FFTW3_DIR}/libfftw3-3.lib")
    
    list(APPEND OPTIONAL_LIBS ${FFTW3_LIBRARY})
    list(APPEND OPTIONAL_INCLUDES ${FFTW3_INCLUDE_DIR})
    list(APPEND COMPILE_DEFINITIONS "USE_FFT")
    
    # Add include directory
    include_directories(${OPTIONAL_INCLUDES})
    
    # Add library directory
    link_directories(${FFTW3_DIR})
endif()

add_executable(main main.cpp)
target_link_libraries(main PRIVATE 
    sfml-graphics 
    sfml-audio
    ${OPTIONAL_LIBS}
)
target_compile_features(main PRIVATE cxx_std_17)
target_compile_definitions(main PRIVATE ${COMPILE_DEFINITIONS})

# Copy FFTW3 DLL only if FFT is enabled
if(USE_FFT)
    add_custom_command(TARGET main POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${FFTW3_DIR}/libfftw3-3.dll"
            $<TARGET_FILE_DIR:main>)
endif()