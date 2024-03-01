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
#include "dirent.h"
#include "pc_tiling.h"

#include <tclap/CmdLine.h>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    std::cout << "Welcolme" << std::endl;

    std::vector<std::string> filenames;
    std::string output_directory;
    int max_verts;
    std::string out_ext;

    try
    {
        // Define the command line object.
        TCLAP::CmdLine cmd("", ' ', "0.9");

        TCLAP::ValueArg<std::string> dirArg("d", "dir", "input directory", false, "", "string");
        TCLAP::ValueArg<std::string> extArg("e", "ext", "output extension [xyz (default) |las]", false, "", "string");
        TCLAP::ValueArg<std::string> fileArg("f", "file", "filename", false, "", "string");
        TCLAP::ValueArg<std::string> outArg("o", "out", "output directory", true, "", "string");
        TCLAP::ValueArg<std::string> maxvArg("v", "verts", "max number of vertex for tile", true, "", "string");
        
        cmd.add(dirArg);
        cmd.add(extArg);
        cmd.add(fileArg);
        cmd.add(outArg); 
        cmd.add(maxvArg);

        // Parse the args.
        cmd.parse(argc, argv);

        if (!fileArg.isSet() && !dirArg.isSet())
        {
            std::cerr << "At least one inbetween input file and input directory MUST be provided" << std::endl;
            return 1;
        }

        if (fileArg.isSet())
            filenames.push_back(fileArg.getValue());

        if (dirArg.isSet())
        {
            if (dirArg.isSet())
            {
                std::cout << "---------------------------------------------" << std::endl;
                std::cout << "Input FILES:" << std::endl;

                DIR* dir;
                struct dirent* ent;
                if ((dir = opendir(dirArg.getValue().c_str())) != NULL)
                {
                    /* print all the files and directories within directory */
                    while ((ent = readdir(dir)) != NULL)
                    {
                        std::string path = dirArg.getValue() + "/" + ent->d_name;

                        int ext_pos = path.find_last_of(".");
                        std::string ext = (ext_pos >= 0) ? path.substr(ext_pos) : "";

                        if (ext.compare(".las") == 0 ||
                            ext.compare(".xyz") == 0)
                        {
                            filenames.push_back(path);
                            std::cout << " --- " << ent->d_name << std::endl;
                        }
                    }
                    closedir(dir);
                }
                else {
                    /* could not open directory */
                    perror("");
                    return EXIT_FAILURE;
                }

                std::cout << "---------------------------------------------" << std::endl;
            }
        }

        output_directory = outArg.getValue();
        max_verts = std::atoi(maxvArg.getValue().c_str());

        if (extArg.isSet())
            out_ext = extArg.getValue();
        else out_ext = "xyz";

        if (out_ext.compare("xyz") != 0 && out_ext.compare("las") != 0)
        {
            std::cerr << "Unsupported output file format: " << out_ext << std::endl;
            return 1;
        }

    }
    catch (exception e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }


    std::vector<std::string> out_filenames;

    OOC3DTileLib::TilingAlgorithms::create_pointcloud_tiling(filenames, output_directory, out_ext, max_verts, out_filenames);
    return 0;
}
