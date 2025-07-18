#ifndef PC_READER_H
#define PC_READER_H

#include "PointCloud.h"
bool read_input_pc(const std::string filename, MiscLib::Vector<Point> &points, double &minx, double &miny, double &minz, double &maxx, double &maxy, double &maxz);

bool read_input_xyz(const std::string filename, MiscLib::Vector<Point> &points,
                    double &minx, double &miny, double &minz,
                    double &maxx, double &maxy, double &maxz);

#endif // PC_READER_H
