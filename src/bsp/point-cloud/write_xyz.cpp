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
#include "write_xyz.h"

void write_bsp_XYZ( BinarySpacePartition &bsp, const std::string out_directory)
{
    for (int leaf=0; leaf < bsp.get_n_leaves(); leaf++)
    {
        BspCell *cell = bsp.get_leaf(leaf);

        if (cell->n_inner_vertices == 0)   
        {
            remove (cell->filename_inner_v.c_str());
            remove (cell->filename_boundary_v.c_str());
            remove (cell->filename_inner_t.c_str());

            continue;   
        }

        // read boundary vertices
        std::set<stxxl::uint64> added_vertices;

        std::ifstream cell_stream (cell->filename_boundary_v.c_str(), std::fstream::in | std::fstream::binary);

        if (!cell_stream.is_open())
        {
            std::cout << "[WARNING] No additional vertices." << std::endl;
        }
        else
        {
            stxxl::uint64 vertex;

            while (cell_stream.read (reinterpret_cast<char *>(&vertex),sizeof(vertex)) && !cell_stream.fail())
            {
                added_vertices.insert(vertex);
            }

            cell_stream.close();
        }

        cell_stream.open(cell->filename_inner_v.c_str());

        if (!cell_stream.is_open())
        {
            std::cout << "[ERROR] Opening file " << cell->filename_inner_v << std::endl;
            exit(1);
        }

        std::string out_filename = out_directory + "cell_" + std::to_string(leaf) + ".xyz";
        std::string local2global_filename = out_directory + "cell_" + std::to_string(leaf) + "_v_loc2glob";

        cell->filename_mesh = out_filename;
        cell->filename_local2global = local2global_filename;

        std::cout << "[OUTPUT] Writing " << out_filename << std::endl;

        std::map <stxxl::uint64, int> global_local_vertices;

        std::ofstream pc_out_stream (out_filename.c_str(), std::fstream::out);
        std::ofstream local2global_out_stream (local2global_filename.c_str(), std::fstream::out);

        if (!pc_out_stream.is_open() || !local2global_out_stream.is_open())
        {
            std::cout << "[ERROR] Opening file " << out_filename <<  " or " << local2global_filename << std::endl;
            exit(1);
        }

        stxxl::uint64 id;
        double x,y,z;

        int vid = 0;

        for (; vid < cell->n_inner_vertices; vid++)
        {
            cell_stream.read (reinterpret_cast<char *>(&id),sizeof(id));

            cell_stream.read (reinterpret_cast<char *>(&x),sizeof(x));
            cell_stream.read (reinterpret_cast<char *>(&y),sizeof(y));
            cell_stream.read (reinterpret_cast<char *>(&z),sizeof(z));

            pc_out_stream << std::setprecision(10) << x << " " << y << " " << z << std::endl;

            global_local_vertices[id] = vid;

            local2global_out_stream << id << std::endl;
        }

        cell_stream.close();

        for (int v : added_vertices)
        {
            pc_out_stream << std::setprecision(10) << bsp.get_point(v).x << " " << bsp.get_point(v).y << " " << bsp.get_point(v).z << std::endl;

            global_local_vertices[v] = vid++;

            local2global_out_stream << v << std::endl;
        }

        cell_stream.open(cell->filename_inner_t.c_str(), std::fstream::in | std::fstream::binary);

        if (!cell_stream.is_open())
        {
            std::cout << "[ERROR] Opening file " << cell->filename_inner_t << std::endl;
            exit(1);
        }

        cell_stream.close();

        pc_out_stream.close();
        local2global_out_stream.close();

        remove (cell->filename_inner_v.c_str());
        remove (cell->filename_boundary_v.c_str());
        remove (cell->filename_inner_t.c_str());
    }
}
