#include <CGAL/Point_set_3.h>
#include <CGAL/Point_set_3/IO.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Shape_detection/Region_growing/Region_growing.h>
#include <CGAL/Shape_detection/Region_growing/Point_set.h>
#include <boost/iterator/function_output_iterator.hpp>
#include "include/utils.h"

#include <tclap/CmdLine.h>


// Typedefs.
using Kernel   = CGAL::Exact_predicates_inexact_constructions_kernel;
using FT       = typename Kernel::FT;
using Point_3  = typename Kernel::Point_3;
using Point_set  = CGAL::Point_set_3<Point_3>;
using Output_range = CGAL::Point_set_3<Point_3>;
using Point_map    = typename Point_set::Point_map;
using Normal_map   = typename Point_set::Vector_map;
using Region_type = CGAL::Shape_detection::Point_set::Least_squares_plane_fit_region_for_point_set<Point_set>;
using Neighbor_query = CGAL::Shape_detection::Point_set::K_neighbor_query_for_point_set<Point_set>;
using Sorting        = CGAL::Shape_detection::Point_set::Least_squares_plane_fit_sorting_for_point_set<Point_set, Neighbor_query>;
using Region_growing = CGAL::Shape_detection::Region_growing<Neighbor_query, Region_type>;
using Point_inserter = utils::Insert_point_colored_by_region_index<Point_set::Index, Output_range, Point_map, Kernel::Plane_3>;

int main(int argc, char *argv[])
{

    std::string input_filename;
    std::string output_filename;

    double angle = 40;
    double distance = 2;
    unsigned int region_size = 5000;
    unsigned int k_par = 12;

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

        TCLAP::ValueArg<std::string> angleArg ("a","max_angle","Max angle [default: 40]",false,"","double");
        TCLAP::ValueArg<std::string> distanceArg ("d","max_distance","Max Distance [default: 2]",false,"","double");
        TCLAP::ValueArg<std::string> sizeArg ("s","region_size","Region Size [default: 5000]",false,"","unsigned integer");
        TCLAP::ValueArg<std::string> kArg ("k","k_neighbors","K-neighbors [default: 12]",false,"","unsigned integer");

        cmd.add( inputFileArg );
        cmd.add( outputFileArg );
        cmd.add( angleArg );
        cmd.add( distanceArg );
        cmd.add( sizeArg );
        cmd.add( kArg );


        // Parse the argv array.
        cmd.parse( argc, argv );

        input_filename = inputFileArg.getValue();
        output_filename = outputFileArg.getValue();

        if (angleArg.isSet())
            angle = std::atof(angleArg.getValue().c_str());

        if (distanceArg.isSet())
            distance = std::atof(distanceArg.getValue().c_str());

        if (sizeArg.isSet())
            region_size = std::atoi(sizeArg.getValue().c_str());

        if (kArg.isSet())
            k_par = std::atoi(kArg.getValue().c_str());


    }
    catch (std::exception e)
    {

    }


    // Load xyz data either from a local folder or a user-provided file.
    std::ifstream in(input_filename);
    CGAL::IO::set_ascii_mode(in);
    if (!in) {
        std::cerr << "ERROR: cannot read the input file!" << std::endl;
        return EXIT_FAILURE;
    }


    const bool with_normal_map = true;
    Point_set point_set(with_normal_map);
    in >> point_set;
    in.close();
    std::cout << "* number of input points: " << point_set.size() << std::endl;
    // assert(!is_default_input || point_set.size() == 8075);


    // Default parameter values for the data file building.xyz.
    const std::size_t k               = k_par;
    const FT          max_distance    = FT(distance);
    const FT          max_angle       = FT(angle);
    const std::size_t min_region_size = region_size;
    // Create instances of the classes Neighbor_query and Region_type.
    Neighbor_query neighbor_query = CGAL::Shape_detection::Point_set::make_k_neighbor_query(
        point_set, CGAL::parameters::k_neighbors(k));
    Sorting sorting = CGAL::Shape_detection::Point_set::make_least_squares_plane_fit_sorting(point_set, neighbor_query);
    sorting.sort();
    Region_type region_type = CGAL::Shape_detection::Point_set::make_least_squares_plane_fit_region(
        point_set,
        CGAL::parameters::
        maximum_distance(max_distance).
        maximum_angle(max_angle).
        minimum_region_size(min_region_size));
    // Create an instance of the region growing class.
    Region_growing region_growing(
        point_set, sorting.ordered(), neighbor_query, region_type);
    // Run the algorithm.
    Output_range output_range;
    std::size_t number_of_regions = 0;
    Point_inserter inserter(
        point_set.point_map(),
        output_range, number_of_regions);
    region_growing.detect(
        boost::make_function_output_iterator(inserter));
    std::cout << "* number of found planes: " << number_of_regions << std::endl;
    // assert(!is_default_input || number_of_regions == 7);
    // Save regions to a file.
    const std::string fullpath =  output_filename;
    std::ofstream out(fullpath);
    out << output_range;
    out.close();
    // Get all unassigned points.
    std::vector<Region_type::Item> unassigned_items;
    region_growing.unassigned_items(point_set, std::back_inserter(unassigned_items));
    std::cout << "* number of unassigned points: " << unassigned_items.size() << std::endl;
    // assert(!is_default_input || unassigned_items.size() == 538);
    // Store all unassigned points.
    std::vector<Point_3> unassigned_points;
    unassigned_points.reserve(unassigned_items.size());
    for (const Region_type::Item &item : unassigned_items) {
        const Point_3& point = get(point_set.point_map(), item);
        unassigned_points.push_back(point);
    }
    return EXIT_SUCCESS;
}
