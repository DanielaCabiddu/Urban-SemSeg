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
#include "bsp.h"
#include "file_manager.h"

void BinarySpacePartition::create(const int max_vtx_per_cell, const std::string out_directory)
{
    std::cout << std::endl << "[BSP] Creating based on vertex downsample ..." << std::endl;

    BspCell *cell = &root;

    int index = 0;

    while (cell->n_inner_vertices > max_vtx_per_cell )
    {
        //std::cout << " - [CELL] " << cell.ID << ": " << cell.n_inner_vertices << " > " << max_vtx_per_cell << std::endl;

        // subdivide the largest grid cell
        // create children and compute their inner points
        split_cell(*cell, out_directory);

        // delete cell from graph
        leaves.erase(leaves.begin() + index);

        //std::cout << " --- [LEFT] " << cell.left->n_inner_vertices << std::endl;
        //std::cout << " --- [RIGHT] " << cell.right->n_inner_vertices << std::endl;

        // add left child
        leaves.push_back(cell->left);

        // add right child
        leaves.push_back(cell->right);

        // update counter
        counter += 2;

        // get the next cell that should be subdivided
        index = get_largest_leaf_by_inner_vertices();
        cell  = leaves.at(index);
    }

    for (int i = 0; i < leaves.size(); i++)
    {
        BspCell *cell = leaves.at(i);

        cell->leaf_ID = i;
        cell->n_inner_vertices = 0;

        remove(cell->filename_inner_v.c_str());
    }

    std::cout << "[BSP] Created. Number of leaves: " << leaves.size() << std::endl << std::endl;
}

void BinarySpacePartition::split_cell (BspCell &cell, const std::string out_directory)
{
    Plane plane = cell.getSubdivisionPlane();

    // create children
    cell.left = new BspCell (plane.min, cell.bbox_max);
    cell.right = new BspCell (cell.bbox_min, plane.max);

    cell.left->parent = &cell;
    cell.right->parent = &cell;

    // set children ids
    cell.left->ID = counter + 1;
    cell.right->ID = counter + 2;

    cell.left->filename_inner_v     = out_directory + "V_cell_"  + std::to_string(cell.left->ID);
    cell.left->filename_inner_t     = out_directory + "T_cell_"  + std::to_string(cell.left->ID);
    cell.left->filename_boundary_v  = out_directory + "BV_cell_" + std::to_string(cell.left->ID);

    cell.right->filename_inner_v     = out_directory + "V_cell_"  + std::to_string(cell.right->ID);
    cell.right->filename_inner_t     = out_directory + "T_cell_"  + std::to_string(cell.right->ID);
    cell.right->filename_boundary_v  = out_directory + "BV_cell_" + std::to_string(cell.right->ID);



    // open children inner vertices file (write mode)
    std::ofstream left_fp (cell.left->filename_inner_v.c_str(), std::ios::out | std::ios::binary);

    if (!left_fp.is_open())
    {
        std::cout << "[ERROR] Opening left child" << std::endl;
        exit(1);
    }

    std::ofstream right_fp (cell.right->filename_inner_v.c_str(), std::ios::out | std::ios::binary);

    if (!right_fp.is_open())
    {
        std::cout << "[ERROR] Opening right child" << std::endl;
        exit(1);
    }

    std::ifstream fp (cell.filename_inner_v.c_str(), std::ios::in | std::ios::binary);

    if (!fp.is_open())
    {
        std::cout << "[ERROR] Opening downsample file" << std::endl;
        exit(1);
    }

    double x= FLT_MAX, y = FLT_MAX, z = FLT_MAX;

    // for each vertex
    for (stxxl::uint64 i = 0; i < cell.n_inner_vertices; i++)
    {
        fp.read (reinterpret_cast<char *>(&x),sizeof(x));
        fp.read (reinterpret_cast<char *>(&y),sizeof(y));
        fp.read (reinterpret_cast<char *>(&z),sizeof(z));

        if (cell.left->hasPoint(x, y, z))
        {
            left_fp.write(reinterpret_cast<const char*>(&x), sizeof x);
            left_fp.write(reinterpret_cast<const char*>(&y), sizeof y);
            left_fp.write(reinterpret_cast<const char*>(&z), sizeof z);

            cell.left->n_inner_vertices++;

            //cell->left->updateMinMaxCoordinates(x, y, z);
        }
        else
        {
            right_fp.write(reinterpret_cast<const char*>(&x), sizeof x);
            right_fp.write(reinterpret_cast<const char*>(&y), sizeof y);
            right_fp.write(reinterpret_cast<const char*>(&z), sizeof z);

            cell.right->n_inner_vertices++;

            //cell.right->updateMinMaxCoordinates(x, y, z);
        }
    }

    fp.close();
    left_fp.close();
    right_fp.close();

    remove(cell.filename_inner_v.c_str());
}


void BinarySpacePartition::fill (const std::string input_binary_filename,
                                const unsigned int n_input_files,
                                bool with_polys)
{
    if (leaves.size() == 0)
        return;

    FileManager file_manager (this);

    assert (file_manager.vOuts.size() == leaves.size());
    assert (file_manager.tOuts.size() == leaves.size());
    assert (file_manager.bvOuts.size() == leaves.size());

    BspCell *cell = leaves.at(0);

    std::ifstream binary_mesh (input_binary_filename.c_str(), std::ios::in | std::ios::binary);

    if (!binary_mesh.is_open())
    {
        std::cout << "[ERROR] Opening binary file" << input_binary_filename << std::endl;
        exit(1);
    }

    stxxl::uint64 counter = 0;

    for (unsigned int f=0; f < n_input_files; f++)
    {
        stxxl::uint64 n_vertices, n_triangles;

        binary_mesh.read (reinterpret_cast<char *>(&n_vertices),sizeof(n_vertices));
        binary_mesh.read (reinterpret_cast<char *>(&n_triangles),sizeof(n_triangles));

        double x, y, z;

        int curr_cell_pos = 0;
        int curr_cell_id = 0;

        std::cout << "[VERTEX CLASSIFICATION] Running ..." << std::endl;

        stxxl::uint64 perc_v = (stxxl::uint64)(n_vertices / 10);

        // vertex classification
        for (stxxl::uint64 vid = 0; vid < n_vertices; vid++)
        {
            if (perc_v > 0 && ((vid%perc_v) == 0))
                std::cout << " --- --- Reading Vertices .. " << vid << " \\ " << n_vertices << " ( " << (vid / perc_v) * 10 << "% )" << std::endl;

            if (vid == n_vertices - 1)
                std::cout << " --- --- Reading Vertices .. " << vid + 1 << " \\ " << n_vertices << " -- COMPLETED" << std::endl;

            // read point
            binary_mesh.read (reinterpret_cast<char *>(&x),sizeof(x));
            binary_mesh.read (reinterpret_cast<char *>(&y),sizeof(y));
            binary_mesh.read (reinterpret_cast<char *>(&z),sizeof(z));

            // search for the corresponding cell
            if (!cell->hasPoint(x, y, z))
            {
                cell = &root;

                while (cell->left != NULL)
                {
                    if (cell->left->hasPoint(x, y, z))
                        cell = cell->left;
                    else cell = cell->right;
                }

                curr_cell_pos = cell->leaf_ID;
                curr_cell_id = cell->ID;
            }

            file_manager.write_vertex(curr_cell_pos, counter, x, y, z);     // write inner vertex into the inner vertex file of the current cell

            counter++;
            cell->n_inner_vertices++;

            if (with_polys)
            {
                vtx2cell.push_back(curr_cell_pos);              // mapping vertex --> bsp_cell
                vtx2boundary.push_back(UNKNOWN_BOUNDARY_INFO);  // no information about "is it on the boundary of current cell?"

                Point point;
                point.x = x;
                point.y = y;
                point.z = z;

                input_coords.push_back(point);
            }
        }

        std::cout << "[VERTEX CLASSIFICATION] Completed." << std::endl << std::endl;


        if (with_polys)
        {
            std::cout << "[TRIANGLE CLASSIFICATION] Running ..." << std::endl;

            stxxl::uint64 perc_t = (stxxl::uint64)(n_triangles / 10);

            stxxl::uint64 v1 = UINT64_MAX, v2 = UINT64_MAX, v3 = UINT64_MAX;

            // triangle classification
            for (stxxl::uint64 i = 0; i < n_triangles; i++)
            {
                if (perc_t > 0 && ((i%perc_t) == 0))
                    std::cout << " --- --- Reading Triangles .. " << i << " \\ " << n_triangles << " ( " << (i / perc_t) * 10 << "% )" << std::endl;

                if (i == n_triangles - 1)
                    std::cout << " --- --- Reading Triangles .. " << i + 1 << " \\ " << n_triangles << " -- COMPLETED" << std::endl;

                binary_mesh.read (reinterpret_cast<char *>(&v1),sizeof(v1));
                binary_mesh.read (reinterpret_cast<char *>(&v2),sizeof(v2));
                binary_mesh.read (reinterpret_cast<char *>(&v3),sizeof(v3));

                assert (v1 != v2);assert (v1 != v3);assert (v3 != v2);

                std::pair<int, int> selected;
                std::pair<int, int> neighbor1, neighbor2;

                classify_triangle(v1, v2, v3, selected, neighbor1, neighbor2);   // decide the triangle clasfficication (the cell where it lies)

                assert (v1 != v2);assert (v1 != v3);assert (v3 != v2);

                file_manager.write_triangle(selected.first, v1, v2, v3);     // write inner triangle into the inner triangle file of the current cell

                leaves.at(selected.first)->n_inner_triangles++;     // update the triangle counter of the selected cell

                std::vector<std::pair<int,int>> neighbor_info;

                if (neighbor1.first != -1)     // the triangle partially lies outside the selected cell
                {
                    neighbor_info.push_back(neighbor1);

                    if (neighbor2.first != -1)
                        neighbor_info.push_back(neighbor2);
                }

                for (int i=0; i < neighbor_info.size(); i++)
                {
                    // add external boundary vertex to boundary vertex file
                    file_manager.write_boundary_vertex(selected.first, neighbor_info.at(i).second);

                    // update neighbor info in the bsp
                    if (vtx2boundary.at(neighbor_info.at(i).second) == UNKNOWN_BOUNDARY_INFO)
                    {
                        vtx2boundary.at(neighbor_info.at(i).second) = selected.first;
                    }
                    else if (vtx2boundary.at(neighbor_info.at(i).second) >= 0 && vtx2boundary.at(neighbor_info.at(i).second) != selected.first)
                    {
                        ConstrainedVertex constrained_vertex;
                        constrained_vertex.vid = neighbor_info.at(i).second;
                        constrained_vertex.cells.insert(vtx2boundary.at(neighbor_info.at(i).second));
                        constrained_vertex.cells.insert(selected.first);

                        constrained_vertices[neighbor_info.at(i).second] = (constrained_vertex);

                        vtx2boundary.at(neighbor_info.at(i).second) = CONSTRAINED_BOUNDARY_VERTEX; // constrained: it is shared among more than 2
                    }
                    else if (vtx2boundary.at(neighbor_info.at(i).second) == CONSTRAINED_BOUNDARY_VERTEX)
                    {
                        constrained_vertices.find(neighbor_info.at(i).second)->second.cells.insert(selected.first);
                    }

                    cell->neighbor_bsp_cells.insert(neighbor_info.at(i).first);
                    leaves.at(neighbor_info.at(i).first)->neighbor_bsp_cells.insert(selected.first);
                }
            }

            std::cout << "[TRIANGLE CLASSIFICATION] Completed.. " << std::endl << std::endl;
        }
    }

    file_manager.close_all();

}

const int BinarySpacePartition::get_largest_leaf_by_inner_vertices() const
{
    int n_cells = leaves.size();

    stxxl::uint64 n_points = 0;
    int index = -1;

    for (int i=0; i < n_cells; i++)
    {
        stxxl::uint64 n = leaves.at(i)->n_inner_vertices;

        if (n > n_points)
        {
            n_points = n;
            index = i;
        }
    }

    return index;
}

const BspCell & BinarySpacePartition::get_minimum_cell (const BspCell &a, const BspCell &b) const
{
    if (a < b)
        return a;

    return b;
}

void BinarySpacePartition::classify_triangle (const stxxl::uint64 v1, const stxxl::uint64 v2, const stxxl::uint64 v3,
                                                std::pair<int, int> &selected_cell_vtx, std::pair<int, int> &neighbor_cell_vtx_1, std::pair<int, int> &neighbor_cell_vtx_2)
{
    neighbor_cell_vtx_1.first = neighbor_cell_vtx_1.second = -1;
    neighbor_cell_vtx_2.first = neighbor_cell_vtx_2.second = -1;

    // get the cell of three vertices
    int cell_pos_v1 = vtx2cell.at(v1);
    int cell_pos_v2 = vtx2cell.at(v2);
    int cell_pos_v3 = vtx2cell.at(v3);

    if (cell_pos_v1 == cell_pos_v2)
    {
        selected_cell_vtx.first = cell_pos_v1;
        selected_cell_vtx.second = v1;

        if (cell_pos_v3 != cell_pos_v1)
        {
            neighbor_cell_vtx_1.first = cell_pos_v3;
            neighbor_cell_vtx_1.second = v3;
        }            
    }
    else if (cell_pos_v2 == cell_pos_v3)
    {
        selected_cell_vtx.first = cell_pos_v2;
        selected_cell_vtx.second = v2;

        if (cell_pos_v1 != cell_pos_v2)
        {
            neighbor_cell_vtx_1.first = cell_pos_v1;
            neighbor_cell_vtx_1.second = v1;
        }
    }
    else if (cell_pos_v3 == cell_pos_v1)
    {
        selected_cell_vtx.first = cell_pos_v3;
        selected_cell_vtx.second = v3;

        if (cell_pos_v2 != cell_pos_v3)
        {
            neighbor_cell_vtx_1.first = cell_pos_v2;
            neighbor_cell_vtx_1.second = v2;
        }
    }
    else
    {
        BspCell min = get_minimum_cell(*(leaves.at(cell_pos_v1)), get_minimum_cell(*(leaves.at(cell_pos_v2)), *(leaves.at(cell_pos_v3))));

        selected_cell_vtx.first = min.leaf_ID;

        if (selected_cell_vtx.first == cell_pos_v1)
        {
            selected_cell_vtx.second = v1;

            neighbor_cell_vtx_1.first = cell_pos_v2;
            neighbor_cell_vtx_1.second = v2;

            neighbor_cell_vtx_2.first = cell_pos_v3;
            neighbor_cell_vtx_2.second = v3;
        }
        else
        if (selected_cell_vtx.first == cell_pos_v2)
        {
            selected_cell_vtx.second = v2;

            neighbor_cell_vtx_1.first = cell_pos_v1;
            neighbor_cell_vtx_1.second = v1;

            neighbor_cell_vtx_2.first = cell_pos_v3;
            neighbor_cell_vtx_2.second = v3;
        }
        else
        if (selected_cell_vtx.first == cell_pos_v3)
        {
            selected_cell_vtx.second = v3;

            neighbor_cell_vtx_1.first = cell_pos_v1;
            neighbor_cell_vtx_1.second = v1;

            neighbor_cell_vtx_2.first = cell_pos_v2;
            neighbor_cell_vtx_2.second = v2;
        }
        else
        {
            std::cerr << "[ERROR] Something went wrong while classifying triangle ["
                      << v1 << " , " << v2 << " , " << v3 << "]." << std::endl;

            exit(1);
        }
    }
}
