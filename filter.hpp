//
// Created by rainy on 2020/5/25.
//

#ifndef COURSE_PROJECT_FILTER_HPP
#define COURSE_PROJECT_FILTER_HPP

void gaussianBlur(Mat &, Mat &);

float** gaussKernel(int, float);

void deleteKernel(float**, int);

void anisotropic(Mat &srcImg, Mat &dstImg, int iter, float k, float lambda);

#endif //COURSE_PROJECT_FILTER_HPP
