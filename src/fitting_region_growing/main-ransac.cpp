#include <tclap/CmdLine.h>

#include <fstream>
#include <iostream>
#include <CGAL/property_map.h>
#include <CGAL/IO/read_points.h>
#include <CGAL/Point_with_normal_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Shape_detection/Efficient_RANSAC.h>
// Type declarations.
typedef CGAL::Exact_predicates_inexact_constructions_kernel  Kernel;
typedef Kernel::FT                                           FT;
typedef std::pair<Kernel::Point_3, Kernel::Vector_3>         Point_with_normal;
typedef std::vector<Point_with_normal>                       Pwn_vector;
typedef CGAL::First_of_pair_property_map<Point_with_normal>  Point_map;
typedef CGAL::Second_of_pair_property_map<Point_with_normal> Normal_map;
typedef CGAL::Shape_detection::Efficient_RANSAC_traits
    <Kernel, Pwn_vector, Point_map, Normal_map>             Traits;
typedef CGAL::Shape_detection::Efficient_RANSAC<Traits> Efficient_ransac;
typedef CGAL::Shape_detection::Cone<Traits>             Cone;
typedef CGAL::Shape_detection::Cylinder<Traits>         Cylinder;
typedef CGAL::Shape_detection::Plane<Traits>            Plane;
typedef CGAL::Shape_detection::Sphere<Traits>           Sphere;
typedef CGAL::Shape_detection::Torus<Traits>            Torus;


int main (int argc, char** argv)
{

    std::string input_filename;
    std::string output_directory;

    bool detect_plane = false;
    bool detect_cylinder = false;
    bool detect_sphere = false;
    bool detect_cone = false;
    bool detect_torus = false;

    float m_epsilon = .005;
    float m_bitmapEpsilon = .01;
    float m_normalThresh = .906307787;
    float m_minSupport = .005;
    float m_probability = .01;

    try {

        // Define the command line object, and insert a message
        // that describes the program. The "Command description message"
        // is printed last in the help text. The second argument is the
        // delimiter (usually space) and the last one is the version number.
        // The CmdLine object parses the argv array based on the Arg objects
        // that it contains.
        TCLAP::CmdLine cmd("", ' ', "0.9");

        TCLAP::ValueArg<std::string> inputFileArg ("i","input","Input File",true,"","string");
        TCLAP::ValueArg<std::string> outputDirArg ("o","output","Output Directory",true,"","string");

        TCLAP::SwitchArg planeSwitch("P","plane","Detect planes",false);
        TCLAP::SwitchArg cylinderSwitch("C","cylinder","Detect cylinder",false);
        TCLAP::SwitchArg sphereSwitch("S","sphere","Detect sphere",false);
        TCLAP::SwitchArg coneSwitch("N","cone","Detect cone",false);
        TCLAP::SwitchArg torusSwitch("T","torus","Detect torus",false);

        TCLAP::ValueArg<std::string> epsArg ("e","epsilon","",false,"","float");
        TCLAP::ValueArg<std::string> bitmapArg ("b","bitmap","",false,"","float");
        TCLAP::ValueArg<std::string> normalArg ("n","normal","",false,"","float");
        TCLAP::ValueArg<std::string> supportArg ("s","support","",false,"","float");
        TCLAP::ValueArg<std::string> probabilityArg ("p","probability","",false,"","float");


        cmd.add( inputFileArg );
        cmd.add( outputDirArg );
        cmd.add( planeSwitch );
        cmd.add( cylinderSwitch );
        cmd.add( sphereSwitch );
        cmd.add( coneSwitch );
        cmd.add( torusSwitch );

        cmd.add(epsArg);
        cmd.add(bitmapArg);
        cmd.add(normalArg);
        cmd.add(supportArg);
        cmd.add(probabilityArg);

        // Parse the argv array.
        cmd.parse( argc, argv );

        input_filename = inputFileArg.getValue();
        output_directory = outputDirArg.getValue();

        detect_plane = planeSwitch.isSet();
        detect_cylinder = cylinderSwitch.isSet();
        detect_sphere = sphereSwitch.isSet();
        detect_cone = coneSwitch.isSet();
        detect_torus = torusSwitch.isSet();

        if (!detect_plane && !detect_cylinder && !detect_sphere && !detect_cone && !detect_torus)
        {
            std::cerr << "Error. Please specify at least one geometry to be detected." << std::endl;
            return 2;
        }

        if (epsArg.isSet())
            m_epsilon = std::atof(epsArg.getValue().c_str());

        if (bitmapArg.isSet())
            m_bitmapEpsilon = std::atof(bitmapArg.getValue().c_str());

        if (normalArg.isSet())
            m_normalThresh = std::atof(normalArg.getValue().c_str());

        if (supportArg.isSet())
            m_minSupport = std::atof(supportArg.getValue().c_str());

        if (probabilityArg.isSet())
            m_probability = std::atof(probabilityArg.getValue().c_str());
    }
    catch (std::exception e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // Points with normals.
    Pwn_vector points;
    // Load point set from a file.
    if (!CGAL::IO::read_points(
            input_filename,
            std::back_inserter(points),
            CGAL::parameters::point_map(Point_map()).
            normal_map(Normal_map()))) {
        std::cerr << "Error: cannot read file " << input_filename << std::endl;
        return EXIT_FAILURE;
    }

    std::cout << "Efficient RANSAC" << std::endl;

    // Instantiate shape detection engine.
    Efficient_ransac ransac;
    // Provide input data.
    ransac.set_input(points);

    // Register shapes for detection.
    ransac.add_shape_factory<Plane>();
    ransac.add_shape_factory<Sphere>();
    ransac.add_shape_factory<Cylinder>();
    ransac.add_shape_factory<Cone>();
    ransac.add_shape_factory<Torus>();

    // Set parameters for shape detection.
    Efficient_ransac::Parameters parameters;
    // Set probability to miss the largest primitive at each iteration.
    parameters.probability = 0.05;
    // Detect shapes with at least 200 points.
    parameters.min_points = 200;
    // Set maximum Euclidean distance between a point and a shape.
    parameters.epsilon = 0.002;
    // Set maximum Euclidean distance between points to be clustered.
    parameters.cluster_epsilon = 0.01;
    // Set maximum normal deviation.
    // 0.9 < dot(surface_normal, point_normal);
    parameters.normal_threshold = 0.9;

    // Detect registered shapes with default parameters.
    ransac.detect();
    // Print number of detected shapes.
    std::cout << ransac.shapes().end() - ransac.shapes().begin()
              << " shapes detected." << std::endl;
    return EXIT_SUCCESS;


}
