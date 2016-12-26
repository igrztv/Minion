#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

bool read_file(char *file_name, vector<Point> &points);