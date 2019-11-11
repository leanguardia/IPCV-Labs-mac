#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main() {

  Mat image = imread("mandrill3.jpg", 1);

  for(int y=0; y<image.rows; y++) {
    for(int x=0; x<image.cols; x++) {
      uchar blue = image.at<Vec3b>(y,x)[0]; // Blue
      uchar green = image.at<Vec3b>(y,x)[1]; // Green
      uchar red = image.at<Vec3b>(y,x)[2]; // Red

      // TODO: Transform RGB to HSV
    }
  }

  imwrite("reconstruct3.jpg", image);

  return 0;
}