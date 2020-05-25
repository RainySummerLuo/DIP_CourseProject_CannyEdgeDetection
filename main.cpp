#include <opencv2/opencv.hpp>
#include "io.hpp"
#include "courseproject.hpp"

using namespace cv;

int main() {
    char srcFile[] = "../pic/lena.png";
    string srcPicFileName = srcFile;
    Mat srcImg = imgRead(srcFile);
    imgCannyEdge(srcImg);
    return 0;
}
