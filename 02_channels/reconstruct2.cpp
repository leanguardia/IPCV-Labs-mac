#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

uchar abs(uchar x) {
  if (x < 0) x = -x; 
  return x;
}

int main() {
  Mat image = imread("mandrill2.jpg", 1);

  for(int y=0; y<image.rows; y++) {
    for(int x=0; x<image.cols; x++) {
      uchar blue = image.at<Vec3b>(y,x)[0]; // Blue
      uchar green = image.at<Vec3b>(y,x)[1]; // Green
      uchar red = image.at<Vec3b>(y,x)[2]; // Red
      image.at<Vec3b>(y,x)[0] = abs(blue - 255); // Blue
      image.at<Vec3b>(y,x)[1] = abs(green - 255); // Green
      image.at<Vec3b>(y,x)[2] = abs(red - 255); // Red
    }
  }
  imwrite("reconstruct2.jpg", image);

  return 0;
}
