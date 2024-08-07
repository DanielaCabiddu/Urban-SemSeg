#include "pc_reader.h"

#include <PointCloud.h>
#include <RansacShapeDetector.h>
#include <PlanePrimitiveShapeConstructor.h>
#include <CylinderPrimitiveShapeConstructor.h>
#include <SpherePrimitiveShapeConstructor.h>
#include <ConePrimitiveShapeConstructor.h>
#include <TorusPrimitiveShapeConstructor.h>

#include <cfloat>
#include <fstream>
#include <iostream>
#include <tclap/CmdLine.h>

int main(int argc, char **argv)
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

    MiscLib::Vector<Point> points;
    double minx, miny, minz;
    double maxx, maxy, maxz;

    read_input_pc(input_filename, points, minx, miny, minz, maxx, maxy, maxz);

    PointCloud pc;

    Vec3f bbmin, bbmax;

    bbmin.setValue(0,0,0);
    bbmax.setValue(-FLT_MAX,-FLT_MAX,-FLT_MAX);

    // fill or load point cloud from file
    for(unsigned int i=0; i<points.size(); i++)
    {
        pc.push_back(points.at(i));

        if (points.at(i).pos[0] > bbmax[0])
            bbmax[0] = points.at(i).pos[0];

        if (points.at(i).pos[1] > bbmax[1])
            bbmax[1] = points.at(i).pos[1];

        if (points.at(i).pos[2] > bbmax[2])
            bbmax[2] = points.at(i).pos[2];
    }

    // set the bbox in pc
    pc.setBBox(bbmin, bbmax);
    //void calcNormals( float radius, unsigned int kNN = 20, unsigned int maxTries = 100 );
    pc.calcNormals(3);

    std::cout << "==========================" << std::endl;
    std::cout << "Setting RANSAC Options ..." << std::endl;

    RansacShapeDetector::Options ransacOptions;
    ransacOptions.m_epsilon = m_epsilon * pc.getScale() ; // set distance threshold to .01f of bounding box width

    ransacOptions.m_epsilon /= 3;
        // NOTE: Internally the distance threshold is taken as 3 * ransacOptions.m_epsilon!!!
    ransacOptions.m_bitmapEpsilon = m_bitmapEpsilon * pc.getScale(); // set bitmap resolution to .02f of bounding box width
        // NOTE: This threshold is NOT multiplied internally!
    ransacOptions.m_normalThresh = m_normalThresh; // this is the cos of the maximal normal deviation
    ransacOptions.m_minSupport = m_minSupport * points.size(); // this is the minimal numer of points required for a primitive
    ransacOptions.m_probability = m_probability; // this is the "probability" with which a primitive is overlooked

    std::cout << "epsilon: " << ransacOptions.m_epsilon << " [" << m_epsilon << " * " << pc.getScale() << " ]" << std::endl;
    std::cout << "bitmapEpsilon: " << ransacOptions.m_bitmapEpsilon << " [" << m_bitmapEpsilon << " * " << pc.getScale() << " ]" << std::endl;
    std::cout << "normalThresh: " << ransacOptions.m_normalThresh << std::endl;
    std::cout << "minSupport: " << ransacOptions.m_minSupport << " [" << m_minSupport << " * " << points.size() << " ]" << std::endl;


    if (ransacOptions.m_minSupport < 10)
    {
        std::cerr << "point cloud is too small... IGNORED" << std::endl;
        exit(2);
    }



    RansacShapeDetector detector(ransacOptions); // the detector object

    std::cout << "Setting RANSAC Options ... COMPLETED" << std::endl;
    std::cout << "====================================" << std::endl;

    // set which primitives are to be detected by adding the respective constructors
    if (detect_plane)
        detector.Add(new PlanePrimitiveShapeConstructor());

    if (detect_cylinder)
        detector.Add(new CylinderPrimitiveShapeConstructor());

    if (detect_sphere)
        detector.Add(new SpherePrimitiveShapeConstructor());

    if (detect_cone)
        detector.Add(new ConePrimitiveShapeConstructor());

    if (detect_torus)
        detector.Add(new TorusPrimitiveShapeConstructor());

    std::cout << "Running RANSAC Detection ..." << std::endl;

    MiscLib::Vector< std::pair< MiscLib::RefCountPtr< PrimitiveShape >, size_t > > shapes; // stores the detected shapes
    size_t remaining = detector.Detect(pc, 0, pc.size(), &shapes); // run detection
        // returns number of unassigned points
        // the array shapes is filled with pointers to the detected shapes
        // the second element per shapes gives the number of points assigned to that primitive (the support)
        // the points belonging to the first shape (shapes[0]) have been sorted to the end of pc,
        // i.e. into the range [ pc.size() - shapes[0].second, pc.size() )
        // the points of shape i are found in the range
        // [ pc.size() - \sum_{j=0..i} shapes[j].second, pc.size() - \sum_{j=0..i-1} shapes[j].second )

    std::cout << "Running RANSAC Detection ... COMPLETED" << std::endl;

    std::cout << "Remaining Unassigned Points " << remaining << std::endl;

    unsigned int start = pc.size() - shapes[0].second;
    unsigned int end = pc.size();

    for(unsigned int i=0; i<shapes.size(); i++)
    {
        std::string desc;
        shapes[i].first->Description(&desc);

        std::cout << "shape " << i << " consists of " << shapes[i].second << " points, it is a " << desc
                  << " [" << start << ", " << end << "] " << std::endl;

        std::string filename = output_directory + "/" + desc + "_" + std::to_string(i) + ".txt";
        std::ofstream ofile;
        ofile.open(filename);

        if (!ofile.is_open())
        {
            std::cerr << "Error opening " << filename << std::endl;
        }
        else
        {
            for (unsigned int p=0; p < shapes[i].second; p++)
                ofile << std::setprecision(10)
                      << pc.at(start+p).pos[0] + minx << " "
                      << pc.at(start+p).pos[1] + miny << " "
                      << pc.at(start+p).pos[2] + minz << std::endl;

            ofile.close();
        }

        end = start;
        start = end - shapes[i+1].second;
    }
}
