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
#ifndef GEOMETRY_ITEMS_H
#define GEOMETRY_ITEMS_H

#include <stdio.h>

#include <stxxl.h>

class Point {

public:
    double x;
    double y;
    double z;

    Point () {}

};

class Vtx : Point {

public:

    int cell; // position of the grid cell (in the graph) where the vertex is located

    stxxl::uint64 index;

    double x;
    double y;
    double z;

    Vtx () {}

    Vtx(double xx, double yy, double zz)
    {
        x = xx; y = yy; z = zz;
        cell = - 1;
        index = UINT64_MAX;
    }

};

class Plane {
public:

    Vtx min;
    Vtx max;

    int axis; // 0 = x; 1 = y; 2 = z

    Plane () {}

};

typedef struct TriangleStruct {

    int index;
    int v1;
    int v2;
    int v3;

} TriangleStruct;


#endif // GEOMETRY_ITEMS_H
