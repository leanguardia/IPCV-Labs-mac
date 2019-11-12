#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <math.h>

using namespace cv;
using namespace std;

uint hSpace[600][600][250];

void Convolute(
  cv::Mat &input,
  float kernel[3][3],
  cv::Mat &output,
  float derivatives[600][600]
);

void GradientMagnitude(
  float derivativesX[600][600],
  float derivativesY[600][600],
  cv::Mat &output
);

void SegmentByThreshold(
  cv::Mat &input,
  float threshold,
  cv::Mat &output
);

void GradientDirection(
  float derivativesX[600][600],
  float derivativesY[600][600],
  cv::Mat &output
);

Mat HoughSpaceCircles(
  Mat magnitude,
  Mat direction,
  ushort threshold,
  ushort minRad,
  ushort maxRad
);

int main( int argc, char** argv ) {
  char* imageName = argv[1];

  Mat image, gray_image, edgesX, edgesY, magnitude, direction, segment, circles;
  float derivativesX[600][600];
  float derivativesY[600][600];
  float kernelX[3][3] = {{-1, 0, 1},
                         {-2, 0, 2},
                         {-1, 0, 1}};
  float kernelY[3][3] = {{-1, -2, -1},
                         { 0,  0,  0},
                         { 1,  2,  1}};

  image = imread(imageName, 1);
  if(argc != 2 || !image.data) {
    printf(" No image data \n ");
    return -1;
  }
  cvtColor(image, gray_image, COLOR_BGR2GRAY);

  Convolute(gray_image, kernelX, edgesX, derivativesX);
  imwrite("edgesX.jpg", edgesX);

  Convolute(gray_image, kernelY, edgesY, derivativesY);
  imwrite("edgesY.jpg", edgesY);

  magnitude.create(edgesX.size(), edgesX.type());
  GradientMagnitude(derivativesX, derivativesY, magnitude);
  imwrite("magnitude.jpg", magnitude);

  SegmentByThreshold(magnitude, 70.0, segment);
  imwrite("segment.jpg", segment);

  direction.create(edgesX.size(), edgesX.type());
  GradientDirection(derivativesX, derivativesY, direction);
  imwrite("direction.jpg", direction);

  ushort threshold = 225, minRad = 25, maxRad = 150;
  circles = HoughSpaceCircles(segment, direction, threshold, minRad, maxRad);

 return 0;
}

void normalize(float data[600][600], float max, float min, Mat &output) {
  float oldRange = max - min;
  float newMin = 0;
  float newMax = 255;
  float newRange = newMax - newMin;

  for( int i = 0; i < output.rows; i++) {
    for( int j = 0; j < output.cols; j++) {
      float scale = (data[i][j] - min) / oldRange;
      output.at<uchar>(i, j) = (newRange * scale) + newMin;
    }
  }
}

void Convolute(Mat &input, float kernel[3][3], Mat &output, float derivatives[600][600]) {
  output.create(input.size(), input.type());
  int kernelSize = 3;

  int kernelRadiusX = ( kernelSize - 1 ) / 2;
  int kernelRadiusY = ( kernelSize - 1 ) / 2;

  cv::Mat paddedInput;
  cv::copyMakeBorder( input, paddedInput,
    kernelRadiusX, kernelRadiusX, kernelRadiusY, kernelRadiusY,
    cv::BORDER_REPLICATE );

  float min = 10000, max = 10000;

  for ( int i = 0; i < input.rows; i++) {
    for( int j = 0; j < input.cols; j++) {
      float sum = 0.0;
      for( int m = -kernelRadiusX; m <= kernelRadiusX; m++) {
        for( int n = -kernelRadiusY; n <= kernelRadiusY; n++) {
          int imagex = i + m + kernelRadiusX;
          int imagey = j + n + kernelRadiusY;
          int kernelx = m + kernelRadiusX;
          int kernely = n + kernelRadiusY;

          // get the values from the padded image and the kernel
          int imageval = ( int ) paddedInput.at<uchar>( imagex, imagey );
          float kernalval = kernel[kernelx][kernely];
          sum += imageval * kernalval;
        }
      }
      derivatives[i][j] = sum;
      if(sum > max) max = sum;
      if(sum < min) min = sum;
    }
  }
  normalize(derivatives, max, min, output);
}
 
void GradientMagnitude(float derivativesX[600][600], float derivativesY[600][600], cv::Mat &output) {
  float min = 10000, max = -10000;
  float magnitudes[600][600];

  for(int i = 0; i < output.rows; i++) {
    for(int j = 0; j < output.cols; j++) {
      float x_2 = derivativesX[i][j] * derivativesX[i][j];
      float y_2 = derivativesY[i][j] * derivativesY[i][j];
      float magnitude = sqrt(x_2 + y_2);
      magnitudes[i][j] = magnitude;
      if(magnitude > max) max = magnitude;
      if(magnitude < min) min = magnitude; 
    }
  }
  normalize(magnitudes, max, min, output);
}

void SegmentByThreshold(Mat &input, float threshold, Mat &output) {
  output.create(input.size(), input.type());
  for (int i = 0; i < input.rows; i++) {
    for(int j = 0; j < input.cols; j++) {
      if (input.at<uchar>(i, j) >= threshold) {
        output.at<uchar>(i, j) = 255;
      } else {
        output.at<uchar>(i, j) = 0;
      }
    }
  }
}

void GradientDirection(float derivativesX[600][600], float derivativesY[600][600], Mat &output) {
  float min = 999999, max = -999999, direction;
  float directions[600][600];
  for(int i = 0; i < output.rows; i++) {
    for(int j = 0; j < output.cols; j++) {
      direction = atan2(derivativesY[i][j], derivativesX[i][j]);
      direction = direction * 180.0 / 3.14159265;
      directions[i][j] = direction;
      if(direction > max) max = direction;
      if(direction < min) min = direction;
    }
  }
  normalize(directions, max, min, output);
}

Mat HoughSpaceCircles(Mat magnitude, Mat direction, ushort threshold, ushort minRadius, ushort maxRadius) {
  ushort possibleRadious = maxRadius - minRadius, x0, y0;
  Mat houghSpace2D;
  houghSpace2D.create(magnitude.size(), magnitude.type());
  
  cout << "possibleRadious: " << possibleRadious << endl;
  for(int i = 0; i < magnitude.rows; i++) {
    for(int j = 0; j < magnitude.cols; j++) {
      for(int k = 0; k < possibleRadious; k++) {
        hSpace[i][j][k] = 0;
      }
    }
  }
  cout << "hough Space initialized" << endl;
  // for(int i = 1; i < magnitude.rows; i++) {
  //   for(int j = 1; j < magnitude.cols; j++) {
  //     if(magnitude.at<uchar>(i, j) > threshold) {
  //       cout << "for i: " << i << " j: " << j << endl;
  //       for(int r = 0; r < possibleRadious; r++) {
  //         int radius = minRadius + r;
  //         x0 = j + radius * cos(direction.at<uchar>(i, j));
  //         y0 = i + radius * sin(direction.at<uchar>(i, j));
  //         cout << "direction: " << direction.at<uchar>(i, j) " - r: " << r << " radius: " << radius << endl;
  //         cout << "(" << x0 << ", " << y0 << ")"<< endl;
  //         hSpace[x0][y0][r]++;
  //       }
  //     }
  //   }
  // }
  return  houghSpace2D;
}