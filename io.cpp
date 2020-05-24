#include <opencv2/opencv.hpp>
#include "io.hpp"

using namespace std;
using namespace cv;

Mat imgRead(char *filename) {
    Mat image = imread(filename);
    if (!image.data) {
        return Mat();
    }
    return image;
}

int imgSave(const string &filename, const Mat &img) {
    int i = srcFilename.rfind('.');
    int j = srcFilename.rfind('/');
    string path = srcFilename.substr(0, j) + "/output/";
    string name = path + filename + "_" + srcFilename.substr(j + 1);
    if (0 != access(path.c_str(), 0)) {
        // if this folder not exist, create a new one.
        mkdir(path.c_str());
    }
    vector<int> imwrite_params;
    Mat cvtImg;
    string file_ext = toLowerCase(srcFilename.substr(i));
    if (file_ext == ".pgm" || file_ext == ".ppm" || file_ext == ".pbm") {
        imwrite_params.push_back(CV_IMWRITE_PXM_BINARY);
        imwrite_params.push_back(0);
        cvtImg = img.clone();
    } else if (file_ext == ".jpg" || file_ext == ".jpeg") {
        imwrite_params.push_back(CV_IMWRITE_JPEG_QUALITY);
        imwrite_params.push_back(100);
        cvtImg = img.clone();
    } else if (file_ext == ".png") {
        imwrite_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
        imwrite_params.push_back(0);
        cvtImg = img.clone();
    } else {
        cvtImg = img.clone();
    }
    imwrite(name, cvtImg);
    return 0;
}

int pixelSaturation(double value) {
    if (value < 0) {
        return 0;
    } else if (value > 255) {
        return 255;
    } else {
        return (int) cvRound(value);
    }
}

int pixelSaturation(int value) {
    if (value < 0) {
        return 0;
    } else if (value > 255) {
        return 255;
    } else {
        return value;
    }
}

void convertRGB2GRAY(const Mat &srcImg, Mat &dstImg) {
    if (!srcImg.data || srcImg.channels() != 3) {
        return;
    }
    dstImg = Mat::zeros(srcImg.size(), CV_8U);
    uchar *pointSrcImg = srcImg.data;
    uchar *pointDstImg = dstImg.data;
    int stepImage = srcImg.step;
    int stepImageGray = dstImg.step;
    for (int i = 0; i < dstImg.rows; i++) {
        for (int j = 0; j < dstImg.cols; j++) {
            pointDstImg[i * stepImageGray + j] =
                    0.114 * pointSrcImg[i * stepImage + 3 * j] +
                    0.587 * pointSrcImg[i * stepImage + 3 * j + 1] +
                    0.299 * pointSrcImg[i * stepImage + 3 * j + 2];
        }
    }
}