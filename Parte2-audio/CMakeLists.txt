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

include(GNUInstallDirs)
set(PACKAGE_NAME matplotlib_cpp)
set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/${PACKAGE_NAME}/cmake)

# Find Python and required packages
find_package(Python3 COMPONENTS Interpreter Development NumPy REQUIRED)
execute_process(
  COMMAND "${Python3_EXECUTABLE}" -c "import matplotlib"
  RESULT_VARIABLE MATPLOTLIB_FOUND
  OUTPUT_QUIET ERROR_QUIET
)
if(NOT MATPLOTLIB_FOUND EQUAL 0)
  message(FATAL_ERROR "matplotlib not found")
endif()

# Library target
add_library(matplotlib_cpp INTERFACE)
target_include_directories(matplotlib_cpp
  INTERFACE
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/examples>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(matplotlib_cpp INTERFACE
  cxx_std_11
)
target_link_libraries(matplotlib_cpp INTERFACE
  Python3::Python
  Python3::Module
)
if(Python3_NumPy_FOUND)
  target_link_libraries(matplotlib_cpp INTERFACE
    Python3::NumPy
  )
else()
  target_compile_definitions(matplotlib_cpp INTERFACE WITHOUT_NUMPY)
endif()
install(
  TARGETS matplotlib_cpp
  EXPORT install_targets
)

add_executable(main main.cpp)
target_link_libraries(main PRIVATE 
  sfml-graphics 
  sfml-audio 
  matplotlib_cpp
  Python3::Python
  Python3::Module
)
target_compile_features(main PRIVATE cxx_std_17)

set_target_properties(main PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

if(WIN32)
    add_custom_command(
        TARGET main
        COMMENT "Copy OpenAL DLL"
        PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${SFML_SOURCE_DIR}/extlibs/bin/$<IF:$<EQUAL:${CMAKE_SIZEOF_VOID_P},8>,x64,x86>/openal32.dll $<TARGET_FILE_DIR:main>
        VERBATIM)
endif()

# Install headers
install(FILES
  "${PROJECT_SOURCE_DIR}/matplotlibcpp.h"
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# Install targets file
install(EXPORT install_targets
  FILE
    ${PACKAGE_NAME}Targets.cmake
  NAMESPACE
    ${PACKAGE_NAME}::
  DESTINATION
    ${INSTALL_CONFIGDIR}
)

# Install matplotlib_cppConfig.cmake
include(CMakePackageConfigHelpers)
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PACKAGE_NAME}Config.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake
  INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)
install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}Config.cmake
  DESTINATION ${INSTALL_CONFIGDIR}
)