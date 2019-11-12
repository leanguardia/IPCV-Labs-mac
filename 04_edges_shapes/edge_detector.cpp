#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <math.h>

using namespace cv;
using namespace std;

ushort hSpace[810][810][270];

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
  imwrite("out/edgesX.jpg", edgesX);

  Convolute(gray_image, kernelY, edgesY, derivativesY);
  imwrite("out/edgesY.jpg", edgesY);

  magnitude.create(edgesX.size(), edgesX.type());
  GradientMagnitude(derivativesX, derivativesY, magnitude);
  imwrite("out/magnitude.jpg", magnitude);

  SegmentByThreshold(magnitude, 70.0, segment);
  imwrite("out/segment.jpg", segment);

  direction.create(edgesX.size(), edgesX.type());
  GradientDirection(derivativesX, derivativesY, direction);
  imwrite("out/direction.jpg", direction);

  ushort threshold = 225, minRad = 20, maxRad = 145;
  circles = HoughSpaceCircles(segment, direction, threshold, minRad, maxRad);
  imwrite("out/circles.jpg", circles);

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

void resetHoughSpace(short height, short width, short depth) {
for(int i = 0; i < height; i++) {
    for(int j = 0; j < width; j++) {
      for(int k = 0; k < depth; k++) hSpace[i][j][k] = 0;
    }
  }
}

Mat HoughSpaceCircles(Mat magnitude, Mat direction, ushort threshold, ushort minRadius, ushort maxRadius) {
  ushort possibleRadious = maxRadius - minRadius, hSpacePadding = 143, radius, hSpaceWidth, hSpaceHeight;
  int x0, y0, votes;
  int minX = 9999999, maxX = -9999999, minY = 99999999, maxY = -99999999;
  float directionInRadians;
  Mat houghSpace2D;

  cout << "Radius: " << minRadius << " - " << maxRadius << endl;
  resetHoughSpace(magnitude.rows, magnitude.cols, possibleRadious);

  for(int i = 0; i < direction.rows; i++) {
    for(int j = 0; j < direction.cols; j++) {
      if(magnitude.at<uchar>(i, j) > threshold) {
        for(int r = 0; r < possibleRadious; r++) {
          radius = minRadius + r;
          directionInRadians = (float)direction.at<uchar>(i, j) * 0.0243; // scale from 0->255 to 0->2*PI
          x0 = j - radius * cos(directionInRadians);
          y0 = i - radius * sin(directionInRadians);
          // cout << "X0 = " << j << " + " << radius << " * cos(" << directionInRadians << ") = "<< x0 << endl;
          // cout << "Y0 = " << i << " + " << radius << " * sin(" << directionInRadians << ") = "<< y0 << endl;
          if(x0 > maxX) maxX = x0; if(x0 < minX) minX = x0;
          if(y0 > maxY) maxY = y0; if(y0 < minY) minY = y0;
          hSpace[x0 + hSpacePadding][y0 + hSpacePadding][r]++;
        }
      }
    }
  }
  cout << "minX " << minX << ", maxY " << maxX << endl;
  cout << "minY " << minY << ", maxY " << maxY << endl;
  hSpaceWidth = -minX + maxX;
  hSpaceHeight = -minY + maxY;
  cout  << "hSpae width: " << hSpaceWidth << endl;
  cout  << "hSpace height: " << hSpaceHeight << endl;
  houghSpace2D.create(hSpaceHeight, hSpaceWidth, magnitude.type());
  for(int i = 0; i < houghSpace2D.rows; i++) {
    for(int j = 0; j < houghSpace2D.cols; j++) {
      votes = 0;
      for(int r = 0; r < possibleRadious; r++) votes += hSpace[i][j][r];
      houghSpace2D.at<uchar>(i, j) = votes * 2;
    }
  }
  return houghSpace2D;
}