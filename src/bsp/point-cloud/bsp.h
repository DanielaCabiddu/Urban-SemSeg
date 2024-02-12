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

#ifndef BSP_H
#define BSP_H

#include "bsp_cell.h"

#include <set>
#include <vector>

#ifdef USE_CEREAL
#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#endif

#define UNKNOWN_BOUNDARY_INFO -2
#define CONSTRAINED_BOUNDARY_VERTEX -1

struct ConstrainedVertex
{
    stxxl::uint64 vid;

    std::set<int> cells;

};

class BinarySpacePartition
{
private:

    BspCell root;                   // Sampled root node of the bsp. Generated be sampling the input mesh.

    int counter = 0;                // Used to se unique leaf ids

    std::vector<BspCell *> leaves;    // Bsp leaves. Each leaf refers to its files.

    stxxl::vector<int> vtx2cell;
    stxxl::vector<int> vtx2boundary;

    std::vector<Point> input_coords;

    std::map<stxxl::uint64, ConstrainedVertex> constrained_vertices;

    ///////////////////////////
    /// METHODS
    ///////////////////////////

public:

    BinarySpacePartition () {}

    BinarySpacePartition (const BspCell &cell) { root = cell; leaves.push_back(&root);}

    unsigned int get_n_leaves () const { return leaves.size(); }
    BspCell *get_leaf (const unsigned int i) { return leaves.at(i); }

    const Point &get_point (const unsigned int i) { return input_coords.at(i); }

    void create (const int max_vtx_per_cell, const std::string out_directory);
    void fill   (const std::string input_binary_filename, const unsigned intn_input_files, bool with_polys = true);

    void split_cell (BspCell &cell, const std::string out_directory);

    const int get_largest_leaf_by_inner_vertices () const;

    const BspCell & get_minimum_cell (const BspCell &a, const BspCell &b) const; // by lexicographic order of ther barycenters

    void classify_triangle (const stxxl::uint64 v1, const stxxl::uint64 v2, const stxxl::uint64 v3,
                            std::pair<int, int> & selected_cell_vtx, std::pair<int, int> & neighbor_cell_vtx_1, std::pair<int, int> & neighbor_cell_vtx_2);

#ifdef USE_CEREAL

    template <class Archive>
    void serialize( Archive & ar )
    {
        std::vector<BspCell> cells;
        for (unsigned int l=0; l < leaves.size();l++)
            cells.push_back(*(leaves.at(l)));
        ar (CEREAL_NVP(cells));

        cells.clear();
    }

    template <class Archive>
    void deserialize( Archive & ar )
    {
//        ar (CEREAL_NVP(leaves));
    }

#endif

};

#ifndef OOCTRITILELIB_STATIC
#include "bsp.cpp"
#endif

#endif // BSP_H
