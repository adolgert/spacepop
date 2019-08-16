#ifndef BLOB_GDAL_RASTER_H
#define BLOB_GDAL_RASTER_H

#include <array>
#include <exception>
#include <filesystem>
#include <sstream>
#include <vector>

#include "CGAL/Exact_predicates_inexact_constructions_kernel.h"
#include "gdal/gdal_priv.h"
#include "gdal/cpl_conv.h"


namespace spacepop {
    std::shared_ptr<GDALDataset>  OpenGeoTiff(const std::filesystem::path& tiff_file);

    template<typename PointOutputIterator>
    void gdal_raster_points(PointOutputIterator out, GDALRasterBand *band, double cutoff, int corner = 0) {
        typedef CGAL::Exact_predicates_inexact_constructions_kernel Kernel;

        const int x{0}, y{1};
        std::array<int,2> block_size{0, 0};
        band->GetBlockSize(&block_size[x], &block_size[y]);

        std::array<int,2> block_cnt = {
                (band->GetXSize() + block_size[x] - 1) / block_size[x],
                (band->GetYSize() + block_size[y] - 1) / block_size[y],
        };
        if (corner != 0) {
            for (int i=0; i<2; ++i) {
                block_cnt[i] = std::min(block_cnt[i], corner);
            }
        }
        if (band->GetRasterDataType() != GDT_Float64) {
            std::stringstream err;
            err << "Raster had wrong data type " << band->GetRasterDataType();
            throw std::runtime_error(err.str());
        }

        std::vector<double> buffer(block_size[x] * block_size[y]);
        // The subset of the block that is defined near edges of raster.
        std::array<int,2> valid{0, 0};

        for (int y_idx=0; y_idx < block_cnt[y]; y_idx++) {
            for (int x_idx=0; x_idx < block_cnt[x]; x_idx++) {
                auto read_succeed = band->ReadBlock(x_idx, y_idx, &buffer[0]);
                if (read_succeed != CE_None) {
                    std::stringstream read_msg;
                    read_msg << "Could not read block " << read_succeed << " (";
                    read_msg << x_idx << ", " << y_idx << ").";
                    throw std::runtime_error(read_msg.str());
                }
                band->GetActualBlockSize(x_idx, y_idx, &valid[x], &valid[y]);
                for (int pix_y=0; pix_y < valid[y]; pix_y++) {
                    for (int pix_x=0; pix_x < valid[x]; pix_x++) {
                        if (buffer[pix_x + pix_y * block_size[x]] > cutoff) {
                            *out++ = Kernel::Point_2{
                                x_idx * block_size[x] + pix_x,
                                y_idx * block_size[y] + pix_y
                            };
                        }
                    }
                }
            }
        }
    }
}

#endif //BLOB_GDAL_RASTER_H
