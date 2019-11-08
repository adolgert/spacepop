# Tutorials about CMake:
# 1. cliutils.gitlab.io/modern-cmake
# 2. cmake.org
# 3. CMake Cookbook, a book.
cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

find_package(Git QUIET)
if(GIT_FOUND AND EXISTS "${PROJECT_SOURCE_DIR}/.git")
    execute_process(COMMAND ${GIT_EXECUTABLE} describe --tags --always
        OUTPUT_VARIABLE BLOB_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
else()
    message("Needs git in order to tag version")
    set(BLOB_VERSION "0.1")
endif()

# The version string is a tag like "v19.11.01-af98234"
string(REGEX MATCH "[0-9]+.[0-9]+" _numeric_version ${BLOB_VERSION})
message(STATUS "Library version ${_numeric_version}")
project(blob
    VERSION ${_numeric_version}
    DESCRIPTION "Determine patches from a landscape"
    LANGUAGES CXX)

# Set language standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Tell it where to find cmake macros and functions before
# doing includes.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
message(STATUS "Cmake module path ${CMAKE_MODULE_PATH}")

# Includes
include(CheckCXXCompilerFlag)
include(CMakePrintHelpers)

# Default to a release build.
# Use `cmake -DCMAKE_BUILD_TYPE=Debug` to override this.
set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Using native architecture can speed up the code.
# Turn this off if you are using a hodge-podge cluster.
option(COMPILE_NATIVE "Compile for this exact processor" ON)
if(COMPILE_NATIVE)
    check_cxx_compiler_flag("-march=native" _march_native_works)
    set(_CXX_FLAGS)
    if(_march_native_works)
      message(STATUS "Adding -march=native to flags for vectorization")
      set(_CXX_FLAGS "-march=native")
    endif()
endif(COMPILE_NATIVE)


# Exporting makes functions in the library available
# for calling by code outside that library. On Windows,
# it helps to export everything.
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)


#### Find libraries for the source code.
# CGAL and its components
find_package( CGAL QUIET COMPONENTS  )

if ( NOT CGAL_FOUND )

    message(STATUS "This project requires the CGAL library, and will not be compiled.")
    return()

endif()

# include helper file
include( ${CGAL_USE_FILE} )


# Boost and its components. Minimum version specified.
find_package( Boost 1.65 REQUIRED
    COMPONENTS filesystem system program_options
    )

if ( NOT Boost_FOUND )

    message(STATUS "This project requires the Boost library, and will not be compiled.")

    return()

endif()

# Geographic lib
find_package (GeographicLib 1.34 REQUIRED)
include_directories (${GeographicLib_INCLUDE_DIRS})
add_definitions (${GeographicLib_DEFINITIONS})

find_library(
        LIBTIFF
        NAMES tiff
        PATHS /usr/lib/x86_64-linux-gnu
        )


#### Now build the main targets.
add_subdirectory(patches)
add_subdirectory(tests)


#### Installation
# GNU defines standard directories for installation.
include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY
  ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIBDIR ${CMAKE_INSTALL_LIBDIR} CACHE PATH "Installation directory for libraries")
set(INSTALL_BINDIR ${CMAKE_INSTALL_BINDIR} CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDEDIR ${CMAKE_INSTALL_INCLUDEDIR} CACHE PATH "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKEDIR CMake)
else()
  set(DEF_INSTALL_CMAKEDIR share/cmake/${PROJECT_NAME})
endif()
set(INSTALL_CMAKEDIR ${DEF_INSTALL_CMAKEDIR} CACHE PATH "Installation directory for CMake files")

# Tell the user where everything will be installed.
foreach(p LIB BIN INCLUDE CMAKE)
  file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX}/${INSTALL_${p}DIR} _path )
  message(STATUS "Installing ${p} components to ${_path}")
  unset(_path)
endforeach()
