#include "CGAL/IO/write_xyz_points.h"
#include <tclap/CmdLine.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/IO/read_las_points.h>
#include <CGAL/IO/write_ply_points.h>
#include <CGAL/jet_estimate_normals.h>
#include <CGAL/scanline_orient_normals.h>

using Kernel = CGAL::Simple_cartesian<double>;
using Point_3 = Kernel::Point_3;
using Vector_3 = Kernel::Vector_3;
using Point_with_info = std::tuple<Point_3, Vector_3, float, unsigned char>;
using Point_map = CGAL::Nth_of_tuple_property_map<0, Point_with_info>;
using Normal_map = CGAL::Nth_of_tuple_property_map<1, Point_with_info>;
using Scan_angle_map = CGAL::Nth_of_tuple_property_map<2, Point_with_info>;
using Scanline_id_map = CGAL::Nth_of_tuple_property_map<3, Point_with_info>;

void dump (const char* filename, const std::vector<Point_with_info>& points)
{
    std::ofstream ofile (filename /*, std::ios::binary*/);
    ofile.precision(10);
    // CGAL::IO::set_binary_mode(ofile);
    CGAL::IO::write_XYZ
        (ofile, points,
         CGAL::parameters::point_map (Point_map()).
         normal_map (Normal_map()));

}

int main (int argc, char** argv)
{
    std::string fname;// (argc > 1 ? argv[1] : "data/urban.las");
    std::string ofname;

    try {

        // Define the command line object, and insert a message
        // that describes the program. The "Command description message"
        // is printed last in the help text. The second argument is the
        // delimiter (usually space) and the last one is the version number.
        // The CmdLine object parses the argv array based on the Arg objects
        // that it contains.
        TCLAP::CmdLine cmd("", ' ', "0.9");

        TCLAP::ValueArg<std::string> inputFileArg ("i","input","Input File [.las]",true,"","string");
        TCLAP::ValueArg<std::string> outputFileArg ("o","output","Output File [.xyz]",true,"","string");

        cmd.add( inputFileArg );
        cmd.add( outputFileArg );


        // Parse the argv array.
        cmd.parse( argc, argv );

        fname = inputFileArg.getValue();
        ofname = outputFileArg.getValue();
    }
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    std::vector<Point_with_info> points;

    std::cerr << "Reading input file " << fname << std::endl;
    std::ifstream ifile (fname, std::ios::binary);
    if (!ifile ||
        !CGAL::IO::read_LAS_with_properties
        (ifile, std::back_inserter (points),
         CGAL::IO::make_las_point_reader (Point_map()),
         std::make_pair (Scan_angle_map(),
                        CGAL::IO::LAS_property::Scan_angle()),
         std::make_pair (Scanline_id_map(),
                        CGAL::IO::LAS_property::Scan_direction_flag())))
    {
        std::cerr << "Can't read " << fname << std::endl;
        return EXIT_FAILURE;
    }

    std::string fname_no_ext = ofname.substr(0, ofname.find_last_of("."));


    std::cerr << "Estimating normals" << std::endl;
    CGAL::jet_estimate_normals<CGAL::Parallel_if_available_tag>
        (points, 12,
         CGAL::parameters::point_map (Point_map()).
         normal_map (Normal_map()));

    std::cerr << "Orienting normals using scan angle and direction flag" << std::endl;
    CGAL::scanline_orient_normals
        (points,
         CGAL::parameters::point_map (Point_map()).
         normal_map (Normal_map()).
         scan_angle_map (Scan_angle_map()).
         scanline_id_map (Scanline_id_map()));
    dump(std::string(fname_no_ext + "_angle_and_flag.xyz").c_str(), points);

    std::cerr << "Orienting normals using scan direction flag only" << std::endl;
    CGAL::scanline_orient_normals
        (points,
         CGAL::parameters::point_map (Point_map()).
         normal_map (Normal_map()).
         scanline_id_map (Scanline_id_map()));
    dump(std::string(fname_no_ext + "_flag.xyz").c_str(), points);

    std::cerr << "Orienting normals using scan angle only" << std::endl;
    CGAL::scanline_orient_normals
        (points,
         CGAL::parameters::point_map (Point_map()).
         normal_map (Normal_map()).
         scan_angle_map (Scan_angle_map()));
    dump(std::string(fname_no_ext + "_angle.xyz").c_str(), points);

    std::cerr << "Orienting normals using no additional info" << std::endl;
    CGAL::scanline_orient_normals
        (points,
         CGAL::parameters::point_map (Point_map()).
         normal_map (Normal_map()));
    dump(std::string(fname_no_ext + "_nothing.xyz").c_str(), points);

    return EXIT_SUCCESS;
}
