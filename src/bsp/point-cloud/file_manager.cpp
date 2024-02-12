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
#include "file_manager.h"

void FileManager::close_oldest_file()
{
    if (n_open_files == 0)
        return;

    stxxl::uint64 usage = UINT64_MAX;

    int file_type = -1; // 0 -> vOut. 1--> tOut. 2 --> bvOut
    int file_id = -1;

    for (int leaf=0; leaf < vOuts.size(); leaf++)
    {
        if (vOuts.at(leaf)->is_open() && vOuts_usage.at(leaf) < usage)
        {
            usage = vOuts_usage.at(leaf);
            file_type = 0;
            file_id = leaf;
        }

        if (tOuts.at(leaf)->is_open() && tOuts_usage.at(leaf) < usage)
        {
            usage = tOuts_usage.at(leaf);
            file_type = 1;
            file_id = leaf;
        }

        if (bvOuts.at(leaf)->is_open() && bvOuts_usage.at(leaf) < usage)
        {
            usage = bvOuts_usage.at(leaf);
            file_type = 2;
            file_id = leaf;
        }
    }

    if (usage == UINT64_MAX)
        return;

    if (file_type == 0)
        vOuts.at(file_id)->close();
    else
    if (file_type == 1)
        tOuts.at(file_id)->close();
    else
    if (file_type == 2)
        bvOuts.at(file_id)->close();
    else
    {
        std::cerr << "[ERROR] Something went wrong in closing oldest file." << std::endl;
        exit(1);
    }

    n_open_files--;
}

void FileManager::close_all ()
{
    for (int leaf = 0; leaf < bsp->get_n_leaves(); leaf++)
    {
        if (vOuts.at(leaf)->is_open())
        {
           vOuts.at(leaf)->close();
           n_open_files--;
        }

        if (tOuts.at(leaf)->is_open() )
        {
            tOuts.at(leaf)->close();
            n_open_files--;
        }

        if (bvOuts.at(leaf)->is_open())
        {
            bvOuts.at(leaf)->close();
            n_open_files--;
        }
    }

    assert (n_open_files == 0);
}

void FileManager::guarantee_vOut_open  (const int leaf)
{
    if (vOuts.at(leaf)->is_open())
        return;

    if (n_open_files == max_open_file)
        close_oldest_file();

    vOuts.at(leaf)->open (bsp->get_leaf(leaf)->filename_inner_v.c_str(), std::ofstream::app | std::ofstream::binary);

    if (!vOuts.at(leaf)->is_open())
    {
        std::cout << "[ERROR] Opening file " << bsp->get_leaf(leaf)->filename_inner_v << std::endl;
        exit(1);
    }

    n_open_files++;
}

void FileManager::guarantee_tOut_open  (const int leaf)
{
    if (tOuts.at(leaf)->is_open())
        return;

    if (n_open_files == max_open_file)
        close_oldest_file();

    tOuts.at(leaf)->open (bsp->get_leaf(leaf)->filename_inner_t.c_str(), std::ofstream::app | std::ofstream::binary);

    if (!tOuts.at(leaf)->is_open())
    {
        std::cout << "[ERROR] Opening file " << bsp->get_leaf(leaf)->filename_inner_t << std::endl;
        exit(1);
    }

    n_open_files++;
}

void FileManager::guarantee_bvOut_open (const int leaf)
{
    if (bvOuts.at(leaf)->is_open())
        return;

    if (n_open_files == max_open_file)
        close_oldest_file();

    bvOuts.at(leaf)->open (bsp->get_leaf(leaf)->filename_boundary_v.c_str(), std::ofstream::app | std::ofstream::binary);

    if (!bvOuts.at(leaf)->is_open())
    {
        std::cout << "[ERROR] Opening file " << bsp->get_leaf(leaf)->filename_boundary_v << std::endl;
        exit(1);
    }

    n_open_files++;
}

void FileManager::write_vertex (const int leaf, const stxxl::uint64 vid, const double x, const double y, const double z)
{
    guarantee_vOut_open(leaf);

    assert(vOuts.at(leaf)->is_open());

    vOuts.at(leaf)->write(reinterpret_cast<const char*>(&vid), sizeof vid);

    if ((vOuts.at(leaf)->fail()))
    {
        std::cout << "[ERROR] Writing vertex " << vid << " in cell " << bsp->get_leaf(leaf)->ID << std::endl;
        exit(1);
    }

    vOuts.at(leaf)->write(reinterpret_cast<const char*>(&x), sizeof x);

    if ((vOuts.at(leaf)->rdstate() & std::ofstream::badbit) != 0)
    {
        std::cout << "[ERROR] Writing vertex " << vid << " in cell " << bsp->get_leaf(leaf)->ID << std::endl;
        exit(1);
    }

    vOuts.at(leaf)->write(reinterpret_cast<const char*>(&y), sizeof y);

    if ((vOuts.at(leaf)->rdstate() & std::ofstream::badbit) != 0)
    {
        std::cout << "[ERROR] Writing vertex " << vid << " in cell " << bsp->get_leaf(leaf)->ID << std::endl;
        exit(1);
    }

    vOuts.at(leaf)->write(reinterpret_cast<const char*>(&z), sizeof z);

    if ((vOuts.at(leaf)->rdstate() & std::ofstream::badbit) != 0)
    {
        std::cout << "[ERROR] Writing vertex " << vid << " in cell " << bsp->get_leaf(leaf)->ID << std::endl;
        exit(1);
    }

    vOuts_usage.at(leaf) = usage_indicator;

    usage_indicator++;
}

void FileManager::write_boundary_vertex (const int leaf, const stxxl::uint64 vid, const double x, const double y, const double z)
{
    guarantee_bvOut_open(leaf);

    bvOuts.at(leaf)->write(reinterpret_cast<const char*>(&vid), sizeof vid);

    if ((vOuts.at(leaf)->rdstate() & std::ofstream::badbit) != 0)
    {
        std::cout << "[ERROR] Writing boundary vertex " << vid << " in cell " << bsp->get_leaf(leaf)->ID << std::endl;
        exit(1);
    }

    bvOuts.at(leaf)->write(reinterpret_cast<const char*>(&x), sizeof x);

    if ((bvOuts.at(leaf)->rdstate() & std::ofstream::badbit) != 0)
    {
        std::cout << "[ERROR] Writing boundary vertex " << vid << " in cell " << bsp->get_leaf(leaf)->ID << std::endl;
        exit(1);
    }

    bvOuts.at(leaf)->write(reinterpret_cast<const char*>(&y), sizeof y);

    if ((bvOuts.at(leaf)->rdstate() & std::ofstream::badbit) != 0)
    {
        std::cout << "[ERROR] Writing boundary vertex " << vid << " in cell " << bsp->get_leaf(leaf)->ID << std::endl;
        exit(1);
    }

    bvOuts.at(leaf)->write(reinterpret_cast<const char*>(&z), sizeof z);

    if ((bvOuts.at(leaf)->rdstate() & std::ofstream::badbit) != 0)
    {
        std::cout << "[ERROR] Writing boundary vertex " << vid << " in cell " << bsp->get_leaf(leaf)->ID << std::endl;
        exit(1);
    }

    bvOuts_usage.at(leaf) = usage_indicator;

    usage_indicator++;
}

void FileManager::write_boundary_vertex (const int leaf, const stxxl::uint64 vid)
{
    guarantee_bvOut_open(leaf);

    bvOuts.at(leaf)->write(reinterpret_cast<const char*>(&vid), sizeof vid);

    if ((bvOuts.at(leaf)->rdstate() & std::ofstream::badbit) != 0)
    {
        std::cout << "[ERROR] Writing boundary vertex " << vid << " in cell " << bsp->get_leaf(leaf)->ID << std::endl;
        exit(1);
    }

    bvOuts_usage.at(leaf) = usage_indicator;

    usage_indicator++;
}

void FileManager::write_triangle (const int leaf, const stxxl::uint64 v1, const stxxl::uint64 v2, const stxxl::uint64 v3)
{
    guarantee_tOut_open(leaf);

    tOuts.at(leaf)->write(reinterpret_cast<const char*>(&v1), sizeof v1);

    if ((tOuts.at(leaf)->rdstate() & std::ofstream::badbit) != 0)
    {
        std::cout << "[ERROR] Writing triangle in cell " << bsp->get_leaf(leaf)->ID << std::endl;
        exit(1);
    }

    tOuts.at(leaf)->write(reinterpret_cast<const char*>(&v2), sizeof v2);

    if ((tOuts.at(leaf)->rdstate() & std::ofstream::badbit) != 0)
    {
        std::cout << "[ERROR] Writing triangle in cell " << bsp->get_leaf(leaf)->ID << std::endl;
        exit(1);
    }

    tOuts.at(leaf)->write(reinterpret_cast<const char*>(&v3), sizeof v3);

    if ((tOuts.at(leaf)->rdstate() & std::ofstream::badbit) != 0)
    {
        std::cout << "[ERROR] Writing triangle in cell " << bsp->get_leaf(leaf)->ID << std::endl;
        exit(1);
    }

    tOuts_usage.at(leaf) = usage_indicator;

    usage_indicator++;
}
