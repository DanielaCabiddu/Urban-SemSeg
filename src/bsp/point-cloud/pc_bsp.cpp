#include "pc_bsp.h"

#include <liblas/liblas.hpp>

#include <cfloat>

namespace OOC3DTileLib {


inline
    void get_bounding_box_and_downsample_and_binary_LAS (const std::vector<std::string> & pc_filenames,
                                                            const std::string downsample_filename,
                                                            const std::string binary_filename,
                                                            const int percentage,
                                                            stxxl::uint64 &mesh_n_vertices,
                                                            int &mesh_sample_vertices,
                                                            Vtx & bb_min,
                                                            Vtx & bb_max,
                                                            std::vector<stxxl::uint64> &infile2lastv)
{

    bb_min.x = bb_min.y = bb_min.z = DBL_MAX;
    bb_max.x = bb_max.y = bb_max.z = -DBL_MAX;

    infile2lastv.clear();

    char s[256], *line;

    std::cout << "[OPENING] Sample file " << downsample_filename << std::endl;

    FILE *sample_fp = fopen (downsample_filename.c_str(), "wb");

    if (sample_fp == NULL)
    {
        std::cerr << "[ERROR] Opening file " << downsample_filename << std::endl;
        exit(1);
    }

    std::cout << "[OPENING] Binary file " << binary_filename << std::endl;

    std::ofstream binary_mesh (binary_filename.c_str(), std::ios::out | std::ios::binary);

    if (!binary_mesh.is_open())
    {
        std::cerr << "[ERROR] Opening file " << binary_filename << std::endl;
        exit(1);
    }

    stxxl::uint64 managed_v = 0;

    for (int file = 0; file < pc_filenames.size(); file++)
    {
        std::string pc_filename = pc_filenames.at(file);

        std::cout << std::endl;
        std::cout << "---------------------------------------------" << std::endl;
        std::cout << "[OPENING] Point Cloud file " << pc_filename << std::endl;

        std::ifstream pc_file;
        pc_file.open(pc_filename);

        if (!pc_file.is_open())
        {
            std::cerr << "[ERROR] Opening file " << pc_filename << std::endl;
            exit(1);
        }

        liblas::Reader reader (pc_file);

        liblas::Header header = reader.GetHeader();

        bb_min.x = header.GetMinX();
        bb_min.y = header.GetMinY();
        bb_min.z = header.GetMinZ();

        bb_max.x = header.GetMaxX();
        bb_max.y = header.GetMaxY();
        bb_max.z = header.GetMaxZ();

        stxxl::uint64 n_v = header.GetPointRecordsCount();
        stxxl::uint64 n_t = 0;

        binary_mesh.write(reinterpret_cast<const char*>(&n_v), sizeof n_v);
        binary_mesh.write(reinterpret_cast<const char*>(&n_t), sizeof n_t);

        mesh_n_vertices += n_v;
        mesh_sample_vertices = 0;

        stxxl::uint64 perc = (stxxl::uint64)(n_v / 10);

        stxxl::uint64 start = percentage /2;
        stxxl::uint64 sample_ptr = start;

        int delta = -start + (rand() % percentage);

        double coord_buffer[3];

        for (stxxl::uint64 i = 0; i < n_v; i++)
        {
            if ((i%perc) == 0)
                std::cout << " --- --- Reading Vertices .. " << i << " \\ " << n_v << " ( " << (i / perc) * 10 << "% )" << std::endl;

            if (i == n_v - 1)
                std::cout << " --- --- Reading Vertices .. " << i + 1 << " \\ " << n_v << " -- COMPLETED" << std::endl;


            bool success = reader.ReadNextPoint();

            if (!success)
            {
                std::cerr << "EROOOOOOOOOOOOOOOOOOOOOOOOO" << std::endl;
            }

            liblas::Point point = reader.GetPoint();

            coord_buffer[0] = point.GetX();
            coord_buffer[1] = point.GetY();
            coord_buffer[2] = point.GetZ();

            binary_mesh.write(reinterpret_cast<const char*>(&coord_buffer[0]), sizeof coord_buffer[0]);
            binary_mesh.write(reinterpret_cast<const char*>(&coord_buffer[1]), sizeof coord_buffer[1]);
            binary_mesh.write(reinterpret_cast<const char*>(&coord_buffer[2]), sizeof coord_buffer[2]);

            if (i == sample_ptr + delta)
            {
                fwrite((void *) coord_buffer, sizeof(double), 3, sample_fp);

                sample_ptr+= percentage;
                delta = -start + rand() % percentage;

                mesh_sample_vertices++;
            }
        }

        managed_v += n_v;
        infile2lastv.push_back(managed_v-1);
        pc_file.close();
        std::cout << "---------------------------------------------" << std::endl;

    }

    fclose(sample_fp);

    binary_mesh.close();

}

inline
    void get_bounding_box_and_downsample_and_binary_XYZ (const std::vector<std::string> & pc_filenames,
                                                   const std::string downsample_filename,
                                                   const std::string binary_filename,
                                                   const int percentage,
                                                   stxxl::uint64 &mesh_n_vertices,
                                                   int &mesh_sample_vertices,
                                                   Vtx & bb_min,
                                                   Vtx & bb_max)
{
    bb_min.x = bb_min.y = bb_min.z = DBL_MAX;
    bb_max.x = bb_max.y = bb_max.z = -DBL_MAX;

    char s[256], *line;

    std::cout << "[OPENING] Sample file " << downsample_filename << std::endl;

    FILE *sample_fp = fopen (downsample_filename.c_str(), "wb");

    if (sample_fp == NULL)
    {
        std::cerr << "[ERROR] Opening file " << downsample_filename << std::endl;
        exit(1);
    }

    std::cout << "[OPENING] Binary file " << binary_filename << std::endl;

    std::ofstream binary_mesh (binary_filename.c_str(), std::ios::out | std::ios::binary);

    if (!binary_mesh.is_open())
    {
        std::cerr << "[ERROR] Opening file " << binary_filename << std::endl;
        exit(1);
    }

    stxxl::uint64 managed_v = 0;

    for (int file = 0; file < pc_filenames.size(); file++)
    {
        std::string pc_filename = pc_filenames.at(file);

        std::cout << "[OPENING] Point Cloud file " << pc_filename << std::endl;

        std::ifstream fp;
        fp.open(pc_filename.c_str());

        if (!fp.is_open())
        {
            std::cerr << "[ERROR] Opening file " << pc_filename << std::endl;
            exit(1);
        }

        int n = 0;
        stxxl::uint64 n_v = std::count(std::istreambuf_iterator<char>(fp),
                                       std::istreambuf_iterator<char>(), '\n');

        stxxl::uint64 n_t = 0;

        fp.clear();
        fp.seekg(0);



        binary_mesh.write(reinterpret_cast<const char*>(&n_v), sizeof n_v);
        binary_mesh.write(reinterpret_cast<const char*>(&n_t), sizeof n_t);

        mesh_n_vertices = n_v;
        mesh_sample_vertices = 0;

        stxxl::uint64 perc = (stxxl::uint64)(n_v / 10);

        stxxl::uint64 start = percentage /2;
        stxxl::uint64 sample_ptr = start;

        int delta = -start + (rand() % percentage);

        double coord_buffer[3];

        for (stxxl::uint64 i = 0; i < n_v; i++)
        {
            if ((i%perc) == 0)
                std::cout << " --- --- Reading Vertices .. " << i << " \\ " << n_v << " ( " << (i / perc) * 10 << "% )" << std::endl;

            if (i == n_v - 1)
                std::cout << " --- --- Reading Vertices .. " << i + 1 << " \\ " << n_v << " -- COMPLETED" << std::endl;

            fp >> coord_buffer[0] >> coord_buffer[1] >> coord_buffer[2];

            binary_mesh.write(reinterpret_cast<const char*>(&coord_buffer[0]), sizeof coord_buffer[0]);
            binary_mesh.write(reinterpret_cast<const char*>(&coord_buffer[1]), sizeof coord_buffer[1]);
            binary_mesh.write(reinterpret_cast<const char*>(&coord_buffer[2]), sizeof coord_buffer[2]);

            // x
            if (coord_buffer[0] < bb_min.x)
                bb_min.x = coord_buffer[0];

            if (coord_buffer[0] > bb_max.x)
                bb_max.x = coord_buffer[0];

            // y
            if (coord_buffer[1] < bb_min.y)
                bb_min.y = coord_buffer[1];

            if (coord_buffer[1] > bb_max.y)
                bb_max.y = coord_buffer[1];

            // z
            if (coord_buffer[2] < bb_min.z)
                bb_min.z = coord_buffer[2];

            if (coord_buffer[2] > bb_max.z)
                bb_max.z = coord_buffer[2];

            if (i == sample_ptr + delta)
            {
                fwrite((void *) coord_buffer, sizeof(double), 3, sample_fp);

                sample_ptr+= percentage;
                delta = -start + rand() % percentage;

                mesh_sample_vertices++;
            }
        }

        managed_v += n_v;

        fp.close();
    }

    fclose(sample_fp);

    binary_mesh.close();
}

}
