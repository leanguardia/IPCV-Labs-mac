#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main() {
  Mat image = imread("mandrill0.jpg", 1);

  for(int y = 0; y < image.rows; y++) {
    for(int x = 0; x < image.cols; x++) {
      uchar green = image.at<Vec3b>(y, x)[0]; // Blue
      uchar red = image.at<Vec3b>(y, x)[1]; // Green
      uchar blue = image.at<Vec3b>(y, x)[2]; // Red
      image.at<Vec3b>(y, x)[0] = blue;
      image.at<Vec3b>(y, x)[1] = green;
      image.at<Vec3b>(y, x)[2] = red;
    }
  }
  imwrite("reconstruct0.jpg", image);

  return 0;
}
