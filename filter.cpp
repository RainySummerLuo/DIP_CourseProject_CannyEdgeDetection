#include "io.hpp"
#include "filter.hpp"

#define MIN2(a, b) ((a) < (b) ? (a) : (b))
#define MAX2(a, b) ((a) > (b) ? (a) : (b))
#define CLIP3(x, a, b) MIN2(MAX2(a,x), b)

using namespace std;
using namespace cv;

void gaussianBlur(Mat &srcImg, Mat &dstImg) {
    int cols = srcImg.cols;
    int rows = srcImg.rows;
    Mat gasImg(rows, cols, CV_8U, Scalar(0));
    float **kernel = gaussKernel(3, 0.01);

    Mat padImg = Mat(rows + 2, cols + 2, CV_8U, Scalar(0));
    srcImg.copyTo(padImg(Rect(1, 1, cols, rows)));
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            gasImg.at<uchar>(i, j) =
                    kernel[0][0] * (float) padImg.at<uchar>(i - 1, j - 1) +
                    kernel[0][1] * (float) padImg.at<uchar>(i - 1, j) +
                    kernel[0][2] * (float) padImg.at<uchar>(i - 1, j + 1) +
                    kernel[1][0] * (float) padImg.at<uchar>(i, j - 1) +
                    kernel[1][1] * (float) padImg.at<uchar>(i, j) +
                    kernel[1][2] * (float) padImg.at<uchar>(i, j + 1) +
                    kernel[2][0] * (float) padImg.at<uchar>(i + 1, j + 1) +
                    kernel[2][1] * (float) padImg.at<uchar>(i + 1, j) +
                    kernel[2][2] * (float) padImg.at<uchar>(i + 1, j + 1);
        }
    }
    deleteKernel(kernel, 3);
    dstImg = gasImg.clone();
}

float **gaussKernel(int k, float sigma) {
    float **M;
    float sum = 0;
    M = new float *[k];
    for (int i = 0; i < k; i++) {
        M[i] = new float[k];
    }
    for (int i = -(k - 1) / 2; i < (k - 1) / 2 + 1; i++) {
        for (int j = -(k - 1) / 2; j < (k - 1) / 2 + 1; j++) {
            auto f1 = (float) (1.0 / (2 * CV_PI * pow(sigma, 2)));
            auto f2 = (float) (-1 * (pow(i, 2) + pow(j, 2)));
            auto f3 = (float) (f2 / (2 * pow(sigma, 2)));
            M[i + (k - 1) / 2][j + (k - 1) / 2] = f1 * exp(f3);
            sum = sum + M[i + (k - 1) / 2][j + (k - 1) / 2];
        }
    }
    for (int i = 0; i < k; i++) {
        for (int j = 0; j < k; j++) {
            M[i][j] = M[i][j] / sum;
        }
    }
    return M;
}

void deleteKernel(float **M, int k) {
    for (int i = 0; i < k; i++) {
        delete[] M[i];
        M[i] = nullptr;
    }
    delete[] M;
}

void anisotropic(Mat &srcImg, Mat &dstImg, int iter, float k, float lambda) {
    int cols = srcImg.cols;
    int rows = srcImg.rows;
    
    for (int it = 0; it < iter; ++it) {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                int srcPixel = srcImg.at<uchar>(i, j);
                
                if (i == 0 || i == rows - 1 || j == 0 || j == cols - 1) {
                    dstImg.at<uchar>(i, j) = srcPixel;
                    continue;
                }

                float NI = (float) srcImg.at<uchar>(i, j - 1) - (float) srcPixel;
                float EI = (float) srcImg.at<uchar>(i - 1, j) - (float) srcPixel;
                float WI = (float) srcImg.at<uchar>(i + 1, j) - (float) srcPixel;
                float SI = (float) srcImg.at<uchar>(i, j + 1) - (float) srcPixel;

                float cN = exp(-NI * NI / (k * k));
                float cE = exp(-EI * EI / (k * k));
                float cW = exp(-WI * WI / (k * k));
                float cS = exp(-SI * SI / (k * k));

                dstImg.at<uchar>(i, j) = pixelSaturation((float) srcPixel + lambda * (cN * NI + cS * SI + cE * EI + cW * WI));
            }
        }
    }
}