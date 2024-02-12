#ifndef PC_BSP_H
#define PC_BSP_H

#include "geometry_items.h"

#include <string>
#include <vector>

namespace OOC3DTileLib {


void get_bounding_box_and_downsample_and_binary_LAS (const std::vector<std::string> & pc_filenames,
                                                     const std::string downsample_filename,
                                                     const std::string binary_filename,
                                                     const int percentage,
                                                     stxxl::uint64 &mesh_n_vertices,
                                                     int &mesh_sample_vertices,
                                                     Vtx & bb_min,
                                                     Vtx & bb_max, std::vector<stxxl::uint64> &infile2lastv);

void get_bounding_box_and_downsample_and_binary_XYZ (const std::vector<std::string> & mesh_filenames,
                                                    const std::string downsample_filename,
                                                    const std::string binary_filename,
                                                    const int percentage,
                                                    stxxl::uint64 &mesh_n_vertices,
                                                    int &mesh_sample_vertices,
                                                    Vtx & bb_min,
                                                    Vtx & bb_max);

}

#ifndef OOC3DTileLib_STATIC
#include "pc_bsp.cpp"
#endif

#endif // PC_BSP_H
