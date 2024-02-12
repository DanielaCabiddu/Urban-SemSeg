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
#include "bsp_cell.h"

BspCell::BspCell (const Vtx &v1, const Vtx &v2)
{
    this->bbox_min = v1;
    this->bbox_max = v2;
}

const Plane BspCell::getSubdivisionPlane() const {

    Plane plane;

    // get bounding box edge length
    double x_dim = bbox_max.x - bbox_min.x;
    double y_dim = bbox_max.y - bbox_min.y;
    double z_dim = bbox_max.z - bbox_min.z;

    // get the middle point of the largest dimension
    if (std::max(x_dim, std::max(y_dim, z_dim)) == x_dim ) {

        plane.min = Vtx(bbox_min.x + x_dim/2, bbox_min.y, bbox_min.z);
        plane.max = Vtx(bbox_max.x - x_dim/2, bbox_max.y, bbox_max.z);
        plane.axis = 0; // x

    } else if (std::max(x_dim, std::max(y_dim, z_dim)) == y_dim) {

        plane.min = Vtx(bbox_min.x, bbox_min.y + y_dim/2, bbox_min.z);
        plane.max = Vtx(bbox_max.x, bbox_max.y - y_dim/2, bbox_max.z);
        plane.axis = 1; // y

    } else {

        plane.min = Vtx(bbox_min.x, bbox_min.y, bbox_min.z + z_dim/2);
        plane.max = Vtx(bbox_max.x, bbox_max.y, bbox_max.z - z_dim/2);
        plane.axis = 2; // z

    }

    return plane;
}

const bool BspCell::hasPoint (const double x, const double y, const double z) const
{
    if (	x > bbox_min.x
        &&	x < bbox_max.x
        &&	y > bbox_min.y
        &&	y < bbox_max.y
        &&	z > bbox_min.z
        &&	z < bbox_max.z )

        return true;

    return false;
}

