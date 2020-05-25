//
// Created by rainy on 2020/5/25.
//

#ifndef COURSE_PROJECT_FILTER_HPP
#define COURSE_PROJECT_FILTER_HPP

void gaussianBlur(Mat &, Mat &);

float** gaussKernel(int, float);

void deleteKernel(float**, int);

#endif //COURSE_PROJECT_FILTER_HPP
