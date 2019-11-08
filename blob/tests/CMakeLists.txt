
####### GoogleTest Boilerplate
# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# The gtest/gtest_main targets carry header search path
# dependencies automatically when using CMake 2.8.11 or
# later. Otherwise we have to add them here ourselves.
if (CMAKE_VERSION VERSION_LESS 2.8.11)
  include_directories("${gtest_SOURCE_DIR}/include")
endif()
####### GoogleTest Boilerplate END

# Make a second executable that runs the tests.
add_executable(blob_test
    admin_patch_test.cpp
    cgal_tutorial.cpp
    gdal_raster_test.cpp
    projection_test.cpp
    simple_patches_test.cpp
    sparse_settlements_test.cpp
    test_main.cpp
    )

#target_include_directories(blob_test
#        PRIVATE
#        "${gtest_SOURCE_DIR}/include"
#        )
target_include_directories(blob_test
        PRIVATE
        "${CMAKE_SOURCE_DIR}/patches"
        )

target_link_libraries(
        blob_test
        patches
        ${CGAL_LIBRARIES}
        ${CGAL_3RD_PARTY_LIBRARIES}
        geotiff tiff gdal
        Boost::filesystem
        Boost::system
        Boost::program_options
        gtest_main
)

cmake_print_properties(
        TARGETS blob_test
        PROPERTIES COMPILE_OPTIONS COMPILE_DEFINITIONS
                COMPILE_FEATURES COMPILE_FLAGS
                INCLUDE_DIRECTORIES
                INTERFACE_INCLUDE_DIRECTORIES
        )