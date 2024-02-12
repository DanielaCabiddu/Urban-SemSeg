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
#ifndef BSP_CELL_H
#define BSP_CELL_H

#include "geometry_items.h"

#include <stxxl.h>

#include <float.h>
#include <fstream>

#ifdef USE_CEREAL
#include <cereal/types/set.hpp>
#endif

class BspCell {

public:

    bool is_bsp_root  = false;    // True if the cell represents the root of the BSP.

    int ID = 0;         // Identifier of the cell.
    int leaf_ID = 0;    // Referred to the BSP.

    stxxl::uint64 n_inner_vertices   = 0;    // Number of vertices actually lying inside the cell.
    stxxl::uint64 n_inner_triangles  = 0;    // Number of triangles classified as belonging to the cell.

//    stxxl::vector<Vtx> inner_vertices;
//    stxxl::vector<TriangleStruct> inner_triangles;
//    stxxl::vector<stxxl::uint64> bv_vertices;

    stxxl::uint64 usage = 0;     // Indicates for how long the file is open but unused

    Vtx bbox_min = { FLT_MAX,  FLT_MAX,  FLT_MAX};      // extreme minimum vertex
    Vtx bbox_max = {-FLT_MAX, -FLT_MAX, -FLT_MAX};      // extreme maximum vertex

    BspCell *parent = nullptr;     // parent cell
    BspCell *left   = nullptr;     // left child
    BspCell *right  = nullptr;     // right child

    std::set<int> neighbor_bsp_cells;   // Set of cells sharing at least one vertex.
                                        // Each neighbor cell is store by its position in the vector BinarySpacePartition::leaves.

    uint64_t added_bv = 0;

    std::string filename_mesh           = "";
    std::string filename_local2global  = "";

    std::vector<std::string> bufferzone_filenames;

    std::string filename_inner_v    = "";
    std::string filename_inner_t    = "";
    std::string filename_boundary_v = "";

    /////////////////////////////////////////////
    ////// METHODS
    /////////////////////////////////////////////

    BspCell () {}   // Empty constructor
    BspCell (const Vtx & v1, const Vtx & v2);

    ~BspCell() {}   // Deconstructor

    bool operator< (const BspCell & cell) const
    {
        // by lexicographic order of barycenters
        double x = (bbox_min.x + bbox_max.x) /2;
        double y = (bbox_min.y + bbox_max.y) /2;
        double z = (bbox_min.z + bbox_max.z) /2;

        double cell_x = (cell.bbox_min.x + cell.bbox_max.x) /2;
        double cell_y = (cell.bbox_min.y + cell.bbox_max.y) /2;
        double cell_z = (cell.bbox_min.z + cell.bbox_max.z) /2;

        if (x < cell_x) return true;
        if (std::abs (x-cell_x) < 1e-10 && y < cell_y) return true;
        if (z < cell_z) return true;

        return false;
    }

    const Plane getSubdivisionPlane() const;      // get subdivisionPlane

    const bool hasPoint (const double x, const double y, const double z) const;     // check if a point is inside the gridcell (coordinates)

    std::map<uint64_t, Vtx *> getVertexGlobalIndex();    // get global index from file

    // get global index block from file starting from file pointer position
    std::map <uint64_t, Vtx *> getVertexGlobalIndexBlock(FILE **, uint64_t size);

    std::set<stxxl::uint64> getBoundaryVertices();

    bool intersects (BspCell *);

#ifdef USE_CEREAL

    template<class Archive>
    void serialize(Archive & archive)
    {
        archive( CEREAL_NVP(leaf_ID));
        archive( CEREAL_NVP(bbox_min.x), CEREAL_NVP(bbox_min.y), CEREAL_NVP(bbox_min.z) );
        archive( CEREAL_NVP(bbox_max.x), CEREAL_NVP(bbox_max.y), CEREAL_NVP(bbox_max.z) );
        archive( CEREAL_NVP(n_inner_vertices), CEREAL_NVP(n_inner_triangles) );
        archive( CEREAL_NVP(neighbor_bsp_cells) );
    }
#endif

};


#ifndef OOCTRITILELIB_STATIC
#include "bsp_cell.cpp"
#endif


#endif // BSP_CELL_H
