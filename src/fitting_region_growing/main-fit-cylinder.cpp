#include <CGAL/Point_set_3.h>
#include <CGAL/Point_set_3/IO.h>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Shape_detection/Region_growing/Region_growing.h>
#include <CGAL/Shape_detection/Region_growing/Point_set.h>
#include <boost/iterator/function_output_iterator.hpp>

#include "include/utils.h"

#include <tclap/CmdLine.h>


// Typedefs.
using Kernel   = CGAL::Simple_cartesian<double>;
using FT       = Kernel::FT;
using Point_3  = Kernel::Point_3;
using Vector_3 = Kernel::Vector_3;

using Point_set  = CGAL::Point_set_3<Point_3>;
using Point_map  = typename Point_set::Point_map;
using Normal_map = typename Point_set::Vector_map;

using Neighbor_query = CGAL::Shape_detection::Point_set::K_neighbor_query_for_point_set<Point_set>;
using Region_type    = CGAL::Shape_detection::Point_set::Least_squares_cylinder_fit_region_for_point_set<Point_set>;
using Region_growing = CGAL::Shape_detection::Region_growing<Neighbor_query, Region_type>;

int main(int argc, char** argv) {

    std::string input_filename;
    std::string output_filename;

    double angle = 25;
    double distance_n = 1;
    double distance_d = 10;
    unsigned int region_size = 20;
    unsigned int k_par = 20;

    try {

        // Define the command line object, and insert a message
        // that describes the program. The "Command description message"
        // is printed last in the help text. The second argument is the
        // delimiter (usually space) and the last one is the version number.
        // The CmdLine object parses the argv array based on the Arg objects
        // that it contains.
        TCLAP::CmdLine cmd("", ' ', "0.9");

        TCLAP::ValueArg<std::string> inputFileArg ("i","input","Input File",true,"","string");
        TCLAP::ValueArg<std::string> outputFileArg ("o","output","Output File [.ply]",true,"","string");

        TCLAP::ValueArg<std::string> angleArg ("a","max_angle","Max angle [default: 25]",false,"","double");
        TCLAP::ValueArg<std::string> distanceNArg ("n","max_distance_n","Max Distance (numeratore) [default: 1]",false,"","double");
        TCLAP::ValueArg<std::string> distanceDArg ("d","max_distance_d","Max Distance (denominatore) [default: 10]",false,"","double");
        TCLAP::ValueArg<std::string> sizeArg ("s","region_size","Region Size [default: 20]",false,"","unsigned integer");
        TCLAP::ValueArg<std::string> kArg ("k","k_neighbors","K-neighbors [default: 20]",false,"","unsigned integer");

        cmd.add( inputFileArg );
        cmd.add( outputFileArg );
        cmd.add( angleArg );
        cmd.add( distanceNArg );
        cmd.add( distanceDArg );
        cmd.add( sizeArg );
        cmd.add( kArg );


        // Parse the argv array.
        cmd.parse( argc, argv );

        input_filename = inputFileArg.getValue();
        output_filename = outputFileArg.getValue();

        if (angleArg.isSet())
            angle = std::atof(angleArg.getValue().c_str());

        if (distanceNArg.isSet())
            distance_n = std::atof(distanceNArg.getValue().c_str());

        if (distanceDArg.isSet())
            distance_d = std::atof(distanceDArg.getValue().c_str());

        if (sizeArg.isSet())
            region_size = std::atoi(sizeArg.getValue().c_str());

        if (kArg.isSet())
            k_par = std::atoi(kArg.getValue().c_str());


    }
    catch (std::exception e)
    {

    }

    std::ifstream in(input_filename);

    CGAL::IO::set_ascii_mode(in);
    if (!in) {
        std::cerr << "ERROR: cannot read the input file!" << std::endl;
        return EXIT_FAILURE;
    }

    Point_set point_set;
    in >> point_set;
    in.close();
    std::cout << "* number of input points: " << point_set.size() << std::endl;
    // assert(!is_default_input || point_set.size() == 1813);
    assert(point_set.has_normal_map()); // input should have normals

    // Default parameter values for the data file cuble.pwn.
    const std::size_t k               = k_par;
    const FT          max_distance    = FT(distance_n) / FT(distance_d);
    const FT          max_angle       = FT(angle);
    const std::size_t min_region_size = region_size;

    // Create instances of the classes Neighbor_query and Region_type.
    Neighbor_query neighbor_query = CGAL::Shape_detection::Point_set::make_k_neighbor_query(point_set, CGAL::parameters::k_neighbors(k));

    Region_type region_type = CGAL::Shape_detection::Point_set::make_least_squares_cylinder_fit_region(
        point_set,
        CGAL::parameters::
        maximum_distance(max_distance).
        maximum_angle(max_angle).
        minimum_region_size(min_region_size));

    // Create an instance of the region growing class.
    Region_growing region_growing(
        point_set, neighbor_query, region_type);

    // Add maps to get a colored output.
    Point_set::Property_map<unsigned char>
        red   = point_set.add_property_map<unsigned char>("red"  , 0).first,
        green = point_set.add_property_map<unsigned char>("green", 0).first,
        blue  = point_set.add_property_map<unsigned char>("blue" , 0).first;

    // Run the algorithm.
    CGAL::Random random;
    std::size_t num_cylinders = 0;
    region_growing.detect(
        boost::make_function_output_iterator(
            [&](const std::pair< Region_type::Primitive, std::vector<typename Point_set::Index> >& region) {

                // Assign a random color to each region.
                const unsigned char r = static_cast<unsigned char>(random.get_int(64, 192));
                const unsigned char g = static_cast<unsigned char>(random.get_int(64, 192));
                const unsigned char b = static_cast<unsigned char>(random.get_int(64, 192));

                for (const auto &id : region.second) {
                    put(red, id, r);
                    put(green, id, g);
                    put(blue, id, b);
                }
                ++num_cylinders;
            }
            )
        );
    std::cout << "* number of found cylinders: " << num_cylinders << std::endl;
    assert(!is_default_input || num_cylinders == 2);

    // Save regions to a file.
    std::ofstream out(output_filename);
    CGAL::IO::set_ascii_mode(out);
    out << point_set;
    return EXIT_SUCCESS;
}
