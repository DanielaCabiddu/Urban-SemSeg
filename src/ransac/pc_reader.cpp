#include "pc_reader.h"

#include <cfloat>
#include <fstream>

bool read_input_pc (const std::string filename, MiscLib::Vector<Point> &points, Point &min, Point &max)
{
    if (filename.substr(filename.find_first_of(".")).compare(".xyz") == 0)
        return read_input_xyz (filename, points, min, max);

    std::cerr << "Unsupport file format: " << filename << std::endl;
    return false;
}

bool read_input_xyz (const std::string filename, MiscLib::Vector<Point> &points, Point &min, Point &max)
{
    std::ifstream file;
    file.open (filename);

    if (!file.is_open())
    {
        std::cerr << "Error opening " << filename << std::endl;
        return false;
    }

    double x,y,z;

    double minx = DBL_MAX;
    double miny = DBL_MAX;
    double minz = DBL_MAX;

    double maxx = -DBL_MAX;
    double maxy = -DBL_MAX;
    double maxz = -DBL_MAX;

    while (file >> x >> y >> z)
    {
        points.push_back(Point(Vec3f(x,y,z)));

        if (x < minx) minx = x;
        if (y < miny) miny = y;
        if (z < minz) minz = z;

        if (x > maxx) maxx = x;
        if (y > maxy) maxy = y;
        if (z > maxz) maxz = z;
    }

    file.close();

    min.pos.setValue(minx, miny, minz);
    max.pos.setValue(maxx, maxy, maxz);

    std::cout << "Loaded " << points.size() << " points" << std::endl;

    return true;
}
