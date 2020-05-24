#ifndef LAB_IO_HPP
#define LAB_IO_HPP

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

extern string srcFilename;

Mat imgRead(char *);

int imgSave(const string &, const Mat &);

int pixelSaturation(double);

int pixelSaturation(int);

void convertRGB2GRAY(const Mat &, Mat &);

#endif //LAB_IO_HPP
