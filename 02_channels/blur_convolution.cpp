
#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;
using namespace std;

int main() {

  Mat image = imread("mandrill.jpg", 1);
  // cvtColour(image, image, COLOR_BGRA2GRAY);

  Mat result(512, 512, CV_8U, Scalar(0));
  int kernel[3][3] = { {1, 1, 1},
                       {1, 1, 1},
                       {1, 1, 1} };
  float g, norm_factor = 0.111;


  for(int y = 1; y < image.rows -1; y++ ) {
    for(int x = 1; x < image.cols -1; x++ ) {
      g = 0.0;
      for(int m = y - 1; m <= y + 1; m++) {
        for(int n = x - 1; n <= x + 1; n++) {
          g += image.at<uchar>(m, n);
        }
      }
      result.at<uchar>(y, x) = norm_factor * g;
      cout << "("<<y << ", " << x << ") = "<< norm_factor * g<<" - ";
    }
  }

  imwrite("blur.jpg", result);

  return 0;
}