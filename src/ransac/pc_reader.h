#ifndef PC_READER_H
#define PC_READER_H

#include "PointCloud.h"
bool read_input_pc(const std::string filename, MiscLib::Vector<Point> &points, Point &min, Point &max);

bool read_input_xyz(const std::string filename, MiscLib::Vector<Point> &points, Point &min, Point &max);

#endif // PC_READER_H
