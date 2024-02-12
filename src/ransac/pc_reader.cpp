#include "pc_reader.h"

#include <cfloat>
#include <fstream>
#include <vector>

bool read_input_pc (const std::string filename, MiscLib::Vector<Point> &points,
                   double &minx, double &miny, double &minz,
                   double &maxx, double &maxy, double &maxz)
{
    if (filename.substr(filename.find_first_of(".")).compare(".xyz") == 0)
        return read_input_xyz (filename, points, minx, miny, minz, maxx, maxy, maxz);

    std::cerr << "Unsupport file format: " << filename << std::endl;
    return false;
}

bool read_input_xyz (const std::string filename, MiscLib::Vector<Point> &points,
                    double &minx, double &miny, double &minz,
                    double &maxx, double &maxy, double &maxz)
{
    std::ifstream file;
    file.open (filename);

    if (!file.is_open())
    {
        std::cerr << "Error opening " << filename << std::endl;
        return false;
    }

    double x,y,z;

    minx = DBL_MAX;
    miny = DBL_MAX;
    minz = DBL_MAX;

    maxx = -DBL_MAX;
    maxy = -DBL_MAX;
    maxz = -DBL_MAX;

    std::vector<double> xx;
    std::vector<double> yy;
    std::vector<double> zz;

    while (file >> x >> y >> z)
    {
        //points.push_back(Point(Vec3f(x,y,z)));
        xx.push_back(x);
        yy.push_back(y);
        zz.push_back(z);

        if (x < minx) minx = x;
        if (y < miny) miny = y;
        if (z < minz) minz = z;

        if (x > maxx) maxx = x;
        if (y > maxy) maxy = y;
        if (z > maxz) maxz = z;
    }

    file.close();

    for (uint i=0; i < xx.size(); i++)
    {
        points.push_back(Point(Vec3f(xx[i]-minx,yy[i]-miny,zz[i]-minz)));
    }

    std::cout << "Loaded " << points.size() << " points" << std::endl;

    return true;
}
