#ifndef LAB_COURSEPROJECT_HPP
#define LAB_COURSEPROJECT_HPP

int imgCannyEdge(Mat &);

void cannyEdge_opencv(const Mat&, Mat &, double, double, int, bool);

void cannyEdge_opencv_modified(const Mat&, Mat &, double, double, int, bool, int);

void cannyEdge_customized(Mat &, Mat &, double, double, int, bool);

void xyGradient(Mat &, Mat &, bool, const Mat&, const Mat&, double, double);

#endif //LAB_COURSEPROJECT_HPP
