
add_library(patches STATIC)

configure_file(version.h.in version.h)

target_sources(patches
  PRIVATE
    admin_patch.cpp 
    gdal_raster.cpp 
    gdal_vector.cpp
    metis_io.cpp 
    on_demand_raster.cpp
    projection.cpp 
    sparse_settlements.cpp
    split_patches.cpp 
  PUBLIC
        admin_patch.h
        connected_settlements.h
        gdal_raster.h 
        gdal_vector.h 
        metis_io.h
        on_demand_raster.h
        pixel.h
        projection.h 
        read_tiff.h
        shapefile.h
        simple_patches.h
        sparse_settlements.h
        split_patches.h
        version.h
)

# This is here to pick up version.h that is build by configuration.
target_include_directories(patches
        PUBLIC
        "${PROJECT_BINARY_DIR}/patches"
        )

add_executable(blob main.cpp 
        )
add_to_cached_list(CGAL_EXECUTABLE_TARGETS blob)

# Link the executable to CGAL and third-party libraries
target_link_libraries(
        blob
        patches
        ${CGAL_LIBRARIES}
        ${CGAL_3RD_PARTY_LIBRARIES}
        ${GeographicLib_LIBRARIES}
        geotiff tiff gdal
        Boost::filesystem
        Boost::system
        Boost::program_options
) 
