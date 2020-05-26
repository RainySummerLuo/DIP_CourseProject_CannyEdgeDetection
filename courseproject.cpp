#include "io.hpp"
#include "courseproject.hpp"
#include "filter.hpp"

using namespace std;
using namespace cv;

int imgCannyEdge(Mat &srcImg) {
    float threshold1 = 70;
    float threshold2 = 140;
    int apertureSize = 3;
    bool L2gradient = true;
    int filterType = 5;

    int cols = srcImg.cols;
    int rows = srcImg.rows;

    TickMeter tm1, tm2, tm3;

    Mat dstImg_opencv = Mat(rows, cols, CV_8U, Scalar(0));
    tm1.start();
    cannyEdge_opencv(srcImg, dstImg_opencv, threshold1, threshold2, apertureSize, L2gradient);
    tm1.stop();
    cout << "Time elapsed: " << tm1.getTimeMilli() << "ms" << endl;

    Mat dstImg_opencv_modified = Mat(rows, cols, CV_8U, Scalar(0));
    tm2.start();
    cannyEdge_opencv_modified(srcImg, dstImg_opencv_modified, threshold1, threshold2, apertureSize, L2gradient, filterType);
    tm2.stop();
    cout << "Time elapsed: " << tm2.getTimeMilli() << "ms" << endl;

    /*
    Mat dstImg_custom = Mat(rows, cols, CV_8U, Scalar(0));
    tm3.start();
    cannyEdge_customized(srcImg, dstImg_custom, threshold1, threshold2, apertureSize, L2gradient);
    tm3.stop();
    cout << "Time elapsed: " << tm.getTimeMilli() << "ms" << endl;
     */

    imshow("Source Image", srcImg);
    imshow("Result Image - OpenCV", dstImg_opencv);
    imshow("Result Image - OpenCV Modified", dstImg_opencv_modified);
    // imshow("Result Image - Customized", dstImg_custom);
    waitKey(0);
    return 0;
}

void cannyEdge_opencv(const Mat &srcImg, Mat &dstImg, double threshold1, double threshold2, int apertureSize,
                      bool L2gradient) {
    Size size = Size(3, 3);
    Point point = Point(-1, -1);
    int borderType = BORDER_REPLICATE;

    int cols = srcImg.cols;
    int rows = srcImg.rows;
    Mat gryImg = Mat(rows, cols, CV_8U, Scalar(0));
    convertRGB2GRAY(srcImg, gryImg);
    Mat dstImg_edge = Mat(rows, cols, CV_8U, Scalar(0));
    blur(srcImg, dstImg_edge, size, point, borderType);
    Canny(dstImg_edge, dstImg, threshold1, threshold2, apertureSize, L2gradient);
    //srcImg.copyTo(dstImg, dstImg_edge);
}

void cannyEdge_opencv_modified(const Mat &srcImg, Mat &dstImg, double threshold1, double threshold2, int apertureSize,
                               bool L2gradient, int filterType) {
    Size size = Size(apertureSize, apertureSize);
    Point point = Point(-1, -1);
    bool normalize = false;
    int borderType = BORDER_REPLICATE;
    int sigmaX = 0;
    int sigmaY = 0;
    int d = 6;
    int sigmaColor = 175;
    int sigmaSpace = 275;
    int iter = 5;
    int k = 40;
    float lambda = 0.75;

    int cols = srcImg.cols;
    int rows = srcImg.rows;
    Mat gryImg = Mat(rows, cols, CV_8U, Scalar(0));
    convertRGB2GRAY(srcImg, gryImg);
    Mat dstImg_filter = Mat(rows, cols, CV_8U, Scalar(0));
    Mat src_img = gryImg.clone();
    switch (filterType) {
        case 0:
            blur(gryImg, dstImg_filter, size, point, borderType);
            break;
        case 1:
            boxFilter(gryImg, dstImg_filter, -1, size, point, normalize, borderType);
            break;
        case 2:
            GaussianBlur(gryImg, dstImg_filter, size, sigmaX, sigmaY, borderType);
            break;
        case 3:
            medianBlur(gryImg, dstImg_filter, apertureSize);
            break;
        case 4:
            bilateralFilter(gryImg, dstImg_filter, d, sigmaColor, sigmaSpace);
            break;
        case 5:
            anisotropic(src_img, dstImg_filter, iter, k, lambda);
            break;
        default:
            boxFilter(gryImg, dstImg_filter, -1, size, point, false, borderType);
            break;
    }
    imshow("image filter", dstImg_filter);


    Canny(dstImg_filter, dstImg, threshold1, threshold2, apertureSize, L2gradient);
    //srcImg.copyTo(dstImg, dstImg_filter);
}

void cannyEdge_customized(Mat &srcImg, Mat &dstImg, double threshold1, double threshold2, int apertureSize,
                          bool L2gradient) {
    int cols = srcImg.cols;
    int rows = srcImg.rows;
    Mat gryImg = Mat(rows, cols, CV_8U, Scalar(0));
    convertRGB2GRAY(srcImg, gryImg);

    Mat blrImg = Mat(rows, cols, CV_8U, Scalar(0));
    //gaussianBlur(srcImg, gasImg);
    GaussianBlur(gryImg, blrImg, cvSize(apertureSize, apertureSize), 0, 0);

    Mat xygImg = Mat(rows, cols, CV_8U, Scalar(0));
    //Mat xyGrad = Mat(rows, cols, CV_8U, Scalar(0));
    //Mat thGrad = Mat(rows, cols, CV_8U, Scalar(0));
    Mat Sx = (cv::Mat_<double>(3, 3) <<
                                     -1, 0, 1,
            -2, 0, 2,
            -1, 0, 1
    );
    Mat Sy = (cv::Mat_<double>(3, 3) <<
                                     1, 2, 1,
            0, 0, 0,
            -1, -2, -1
    );
    xyGradient(blrImg, xygImg, L2gradient, Sx, Sy, threshold1, threshold2);
    dstImg = xygImg.clone();
}

void xyGradient(Mat &srcImg, Mat &dstImg, bool L2gradient, const Mat &Sx, const Mat &Sy, double min_thresh,
                double max_thresh) {
    int cols = srcImg.cols;
    int rows = srcImg.rows;

    Rect rect;
    rect.width = 3;
    rect.height = 3;
    Mat xGra = Mat(rows, cols, CV_8U, Scalar(0));
    Mat yGra = Mat(rows, cols, CV_8U, Scalar(0));

    for (int i = 0; i < rows - 1; i++) {
        for (int j = 0; j < cols - 1; j++) {
            int a0 = srcImg.at<uchar>(i - 1, j - 1);
            int a1 = srcImg.at<uchar>(i - 1, j);
            int a2 = srcImg.at<uchar>(i - 1, j + 1);
            int a3 = srcImg.at<uchar>(i, j - 1);
            int a4 = srcImg.at<uchar>(i, j);
            int a5 = srcImg.at<uchar>(i, j + 1);
            int a6 = srcImg.at<uchar>(i + 1, j - 1);
            int a7 = srcImg.at<uchar>(i + 1, j);
            int a8 = srcImg.at<uchar>(i + 1, j + 1);
            xGra.at<uchar>(i, j) = abs(
                    -1 * a0 + 0 * a1 + 1 * a2 +
                    -2 * a3 + 0 * a4 + 2 * a5 +
                    -1 * a6 + 0 * a7 + 1 * a8
            );
            yGra.at<uchar>(i, j) = abs(
                    1 * a0 + 2 * a1 + 1 * a2 +
                    0 * a3 + 0 * a4 + 0 * a5 +
                    -1 * a6 + -2 * a7 + -1 * a8
            );
        }
    }

    Mat grdImg = Mat(rows, cols, CV_8U, Scalar(0));
    if (L2gradient) {
        for (int i = 0; i < rows - 1; i++) {
            for (int j = 0; j < cols - 1; j++) {
                grdImg.at<uchar>(i, j) = pixelSaturation(
                        sqrt(pow(xGra.at<uchar>(i, j), 2) + pow(yGra.at<uchar>(i, j), 2)));
            }
        }
    } else {
        for (int i = 0; i < rows - 1; i++) {
            for (int j = 0; j < cols - 1; j++) {
                grdImg.at<uchar>(i, j) = pixelSaturation(abs(xGra.at<uchar>(i, j) + abs(yGra.at<uchar>(i, j))));
            }
        }
    }

    // TODO convertScaleAbs()?
    convertScaleAbs(grdImg, grdImg);

    Mat thdImg = Mat(rows, cols, CV_8U, Scalar(0));
    Mat locImg = Mat(rows, cols, CV_8U, Scalar(0));
    for (int i = 1; i < rows - 1; i++) {
        for (int j = 1; j < cols - 1; j++) {
            thdImg.at<uchar>(i, j) = atan2(yGra.at<uchar>(i, j), xGra.at<uchar>(i, j));
            int thdPixel = thdImg.at<uchar>(i, j);
            if (0 <= thdPixel || thdPixel <= (CV_PI / 4.0)) {
                locImg.at<uchar>(i, j) = 0;
            } else if (CV_PI / 4.0 < thdPixel || thdPixel <= (CV_PI / 2.0)) {
                locImg.at<uchar>(i, j) = 1;
            } else if (-1 * CV_PI / 2.0 <= thdPixel || thdPixel <= (-1 * CV_PI / 4.0)) {
                locImg.at<uchar>(i, j) = 2;
            } else if (-1 * CV_PI / 4.0 < thdPixel || thdPixel < 0) {
                locImg.at<uchar>(i, j) = 3;
            }
        }
    }

    double grdMax;
    minMaxLoc(grdImg, &grdMax);
    if (grdMax != 0) {
        grdImg = grdImg / grdMax;
    }

    Mat xygImg = grdImg.clone();
    Mat nmsImg = Mat(rows, cols, CV_8U, Scalar(0));
    for (int i = 1; i < rows - 1; i++) {
        for (int j = 1; j < cols - 1; j++) {
            // int x = xGra.at<int>(i, j);
            // int y = yGra.at<int>(i, j);
            int a0 = xygImg.at<uchar>(i - 1, j - 1);
            int a1 = xygImg.at<uchar>(i - 1, j);
            int a2 = xygImg.at<uchar>(i - 1, j + 1);
            int a3 = xygImg.at<uchar>(i, j - 1);
            // int a4 = xygImg.at<uchar>(i, j);
            int a5 = xygImg.at<uchar>(i, j + 1);
            int a6 = xygImg.at<uchar>(i + 1, j - 1);
            int a7 = xygImg.at<uchar>(i + 1, j);
            int a8 = xygImg.at<uchar>(i + 1, j + 1);
            double gp1, gp2;

            double angle = abs(tan(thdImg.at<uchar>(i, j)));
            switch ((int) locImg.at<uchar>(i, j)) {
                case 0:
                    gp1 = (1 - angle) * a5 + angle * a2;
                    gp2 = (1 - angle) * a3 + angle * a6;
                    break;
                case 1:
                    gp1 = (1 - angle) * a1 + angle * a2;
                    gp2 = (1 - angle) * a7 + angle * a6;
                    break;
                case 2:
                    gp1 = (1 - angle) * a1 + angle * a0;
                    gp2 = (1 - angle) * a7 + angle * a8;
                    break;
                case 3:
                    gp1 = (1 - angle) * a3 + angle * a0;
                    gp2 = (1 - angle) * a5 + angle * a8;
                    break;
                default:
                    break;
            }

            auto &grdPixel = grdImg.at<uchar>(i, j);
            auto &nmsPixel = nmsImg.at<uchar>(i, j);
            if (grdImg.at<uchar>(i, j) >= gp1 && grdImg.at<uchar>(i, j) >= gp2) {
                if (grdPixel >= max_thresh) {
                    grdPixel = max_thresh;
                    nmsPixel = 255;
                } else if (grdPixel < min_thresh) {
                    grdPixel = 0;
                } else {
                    grdPixel = min_thresh;
                }
            } else {
                grdPixel = 0;
            }
            /*
            double angle = atan2(y, x) * 180 / CV_PI;
            // 0
            if (abs(angle) < 22.5 || abs(angle) >= 157.5) {
                if (a4 >= a3 && a4 >= a5) {
                    nmsPixel = a4;
                } else {
                    nmsPixel = 0;
                }
            }
            // 45
            if (abs(angle) >= 22.5 && abs(angle) < 67.5) {
                if (a4 >= a0 && a4 >= a8) {
                    nmsPixel = a4;
                } else {
                    nmsPixel = 0;
                }
            }
            // 90
            if (abs(angle) >= 67.5 && abs(angle) < 112.5) {
                if (a4 >= a1 && a4 >= a7) {
                    nmsPixel = a4;
                } else {
                    nmsPixel = 0;
                }
            }
            // 135
            if (angle >= 112.5 && angle < 157.5) {
                if (a4 >= a2 && a4 >= a6) {
                    nmsPixel = a4;
                } else {
                    nmsPixel = 0;
                }
            }
            */
        }
    }

    for (int i = 1; i < rows; i++) {
        for (int j = 1; j < cols; j++) {
            int dotPixel = grdImg.at<uchar>(i, j);
            auto &nmsPixel = nmsImg.at<uchar>(i, j);
            if (dotPixel == min_thresh) {
                for (int k = -1; k < 1; k++) {
                    for (int l = -1; l < 1; l++) {
                        if (grdImg.at<uchar>(i + k, j + l) == max_thresh) {
                            nmsPixel = 255;
                            continue;
                        }
                    }
                }
            }
        }
    }
    dstImg = nmsImg.clone();
}