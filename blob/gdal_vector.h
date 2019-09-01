#ifndef BLOB_GDAL_VECTOR_H
#define BLOB_GDAL_VECTOR_H

#include <exception>

#include "boost/filesystem.hpp"

#include "gdal/gdal_priv.h"

namespace spacepop {
    void OpenShapefile(const boost::filesystem::path &shapefile_path);
    void WriteVector(const boost::filesystem::path &shape_filename);
}

#endif //BLOB_GDAL_VECTOR_H
