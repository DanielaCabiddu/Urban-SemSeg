/********************************************************************************
*  This file is part of OOCTriTile                                              *
*  Copyright(C) 2023: Daniela Cabiddu                                           *
*                                                                               *
*  Author(s):                                                                   *
*                                                                               *
*     Daniela Cabiddu (daniela.cabiddu@cnr.it)                                  *
*                                                                               *
*     Italian National Research Council (CNR)                                   *
*     Institute for Applied Mathematics and Information Technologies (IMATI)    *
*     Via de Marini, 6                                                          *
*     16149 Genoa,                                                              *
*     Italy                                                                     *
*                                                                               *
*  This program is free software: you can redistribute it and/or modify it      *
*  under the terms of the GNU General Public License as published by the        *
*  Free Software Foundation, either version 3 of the License, or (at your       *
*  option) any later version.                                                   *
*                                                                               *
*  This program is distributed in the hope that it will be useful, but          *
*  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY   *
*  or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for  *
*  more details.                                                                *
*                                                                               *
*  You should have received a copy of the GNU General Public License along      *
*  with this program. If not, see <https://www.gnu.org/licenses/>.              *
*                                                                               *
*********************************************************************************/
#include "pc_tiling.h"
#include "pc_bsp.h"
#include "write_las.h"
#include "write_xyz.h"


#include "bsp.h"

namespace OOC3DTileLib {

namespace TilingAlgorithms {

void create_pointcloud_tiling(const std::vector<std::string> input_filenames,
                                const std::string              out_directory,
                                const std::string              out_ext,
                                const int                      max_vtx_per_tile,
                                std::vector<std::string>     & tile_filenames)
{
    int bufferzone_size = 0;

    std::vector<std::vector<std::string>> bufferzone_filenames;

    create_pointcloud_tiling(input_filenames, out_directory, out_ext, max_vtx_per_tile, bufferzone_size, tile_filenames, bufferzone_filenames);
}

void create_pointcloud_tiling (const std::vector<std::string> input_filenames,
                                const std::string              out_directory,
                                const std::string              out_ext,
                                const int                      max_vtx_per_tile,
                                const int                      bufferzone_size,
                                std::vector<std::string>     & tile_filenames,
                                std::vector<std::vector<std::string>>     & bufferzone_filenames)
{
#ifndef STXXL
    std::cerr << "[ERROR] STXXL library is necessary to run triangle mesh tiling algorithm." << std::endl;
    exit(1);
#else

    Vtx bb_min;
    Vtx bb_max;

    std::string downsample_filename = out_directory + "/V_downsample";
    std::string binary_filename     = out_directory + "/V_binary";

    stxxl::uint64 n_vertices = 0, n_triangles = 0;
    int n_sample_vertices = 0 ;

    int percentage =  1000;
    stxxl::uint64 stop = max_vtx_per_tile / percentage;

    std::string ext = input_filenames.at(0).substr(input_filenames.at(0).find_last_of("."));

    std::vector<stxxl::uint64> infile2lastv;

    if (ext.compare(".xyz") == 0)
        get_bounding_box_and_downsample_and_binary_XYZ(input_filenames, downsample_filename, binary_filename, percentage,
                                                   n_vertices, n_sample_vertices,
                                                   bb_min, bb_max);
    else
    if (ext.compare(".las") == 0)
        get_bounding_box_and_downsample_and_binary_LAS(input_filenames, downsample_filename, binary_filename, percentage,
                                                   n_vertices, n_sample_vertices,
                                                   bb_min, bb_max, infile2lastv);
    else
    {
        std::cerr << "Unsupported file format: " << ext << std::endl;
        return;
    }

    // Create BSP root by exploiting the vertex downsample
    BspCell root (bb_min, bb_max);
    root.is_bsp_root = true;
    root.n_inner_vertices = n_sample_vertices;
    root.filename_inner_v = downsample_filename;

    // Create BSP starting from the root and exploiting the vertex downsample
    BinarySpacePartition bsp (root);
    bsp.create(stop, out_directory);

    // Fill the BSP cells by reading the original input (both vertices and triangles)
    bsp.fill(binary_filename, input_filenames.size(), false);

    // Write the output according to selected output format
    if (out_ext.compare("xyz") == 0)
        write_bsp_XYZ(bsp, out_directory);
    else
        if (out_ext.compare("las") == 0)
            write_bsp_LAS(bsp, input_filenames, infile2lastv, out_directory);
    else
    {
        std::cerr << "Unsupported output file format: " << out_ext << std::endl;
        return;
    }

    for (int leaf = 0; leaf < bsp.get_n_leaves(); leaf++)
    {
        tile_filenames.push_back(bsp.get_leaf(leaf)->filename_mesh);
    }

#endif
}

}

}
