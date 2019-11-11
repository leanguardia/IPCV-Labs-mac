// header inclusion
#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <math.h>

using namespace cv;
using namespace std;

void Convolute(
	cv::Mat &input,
	double kernel[3][3],
	cv::Mat &output,
  double derivatives[600][600]
);

void Magnitude(
	double derivativesX[600][600],
	double derivativesY[600][600],
	cv::Mat &output
);

void SegmentByThreshold(
	cv::Mat &input,
  double threshold,
	cv::Mat &output
);

void GradientDirection(
	double derivativesX[600][600],
	double derivativesY[600][600],
	cv::Mat &output
);


void HoughSpace(
  Mat magnitude,
  Mat direction,
  int minRad,
  int maxRad,
  float hSpace[600][600][150]
);

int main( int argc, char** argv )
{
  char* imageName = argv[1];

  Mat image, gray_image, edgesX, edgesY, magnitude, direction, segment;
  double derivativesX[600][600];
  double derivativesY[600][600];
  double kernelX[3][3] = {{-1, 0, 1},
                         {-2, 0, 2},
                         {-1, 0, 1}};
  double kernelY[3][3] = {{-1, -2, -1},
                         { 0,  0,  0},
                         { 1,  2,  1}};
  // float hSpace[600][600][150];

  image = imread(imageName, 1 );
  if( argc != 2 || !image.data ) {
    printf( " No image data \n " );
    return -1;
  }
  cvtColor( image, gray_image, COLOR_BGR2GRAY );

  Convolute(gray_image, kernelX, edgesX, derivativesX);
  imwrite( "edgesX.jpg", edgesX );

  Convolute(gray_image, kernelY, edgesY, derivativesY);
  imwrite( "edgesY.jpg", edgesY );

  magnitude.create(edgesX.size(), edgesX.type());
	Magnitude(derivativesX, derivativesY, magnitude);
	imwrite( "magnitude.jpg", magnitude );

	SegmentByThreshold(magnitude, 80.0, segment);
	imwrite( "segment.jpg", segment );

  direction.create(edgesX.size(), edgesX.type());
	GradientDirection(derivativesX, derivativesY, direction);
	imwrite( "direction.jpg", direction );

  // int minRad = 10, maxRad = 270;
  // HoughSpace(magnitude, direction, minRad, maxRad, hSpace);

 return 0;
}

void Convolute(cv::Mat &input, double kernel[3][3], cv::Mat &output, double derivatives[600][600])
{
	output.create(input.size(), input.type());
	int kernelSize = 3;

	int kernelRadiusX = ( kernelSize - 1 ) / 2;
	int kernelRadiusY = ( kernelSize - 1 ) / 2;

	cv::Mat paddedInput;
	cv::copyMakeBorder( input, paddedInput,
		kernelRadiusX, kernelRadiusX, kernelRadiusY, kernelRadiusY,
		cv::BORDER_REPLICATE );

  double min = 10000, max = 10000;

	for ( int i = 0; i < input.rows; i++ )
	{
		for( int j = 0; j < input.cols; j++ )
		{
			double sum = 0.0;
			for( int m = -kernelRadiusX; m <= kernelRadiusX; m++ )
			{
				for( int n = -kernelRadiusY; n <= kernelRadiusY; n++ )
				{
					int imagex = i + m + kernelRadiusX;
					int imagey = j + n + kernelRadiusY;
					int kernelx = m + kernelRadiusX;
					int kernely = n + kernelRadiusY;

					// get the values from the padded image and the kernel
					int imageval = ( int ) paddedInput.at<uchar>( imagex, imagey );
					double kernalval = kernel[kernelx][kernely];
					sum += imageval * kernalval;
				}
			}
      derivatives[i][j] = sum;
      if(sum > max)
      {
        max = sum;
      }
      if(sum < min)
      {
        min = sum;
      }
		}
	}
  double oldrange = max - min;
  double newmin=0;
  double newmax=255;
  double newrange = newmax - newmin;

  for( int i = 0; i < output.rows; i++ )
	{
		for( int j = 0; j < output.cols; j++ )
    {
      double scale = (derivatives[i][j] - min) / oldrange;
      output.at<uchar>(i, j) = (newrange * scale) + newmin;
    }
  }
}
 
void Magnitude(double derivativesX[600][600], double derivativesY[600][600], cv::Mat &output)
{
  double min = 10000, max = -10000;
  double magnitudes[550][550];

	for(int i = 0; i < output.rows; i++)
	{
		for(int j = 0; j < output.cols; j++)
		{
			double x_2 = derivativesX[i][j] * derivativesX[i][j];
			double y_2 = derivativesY[i][j] * derivativesY[i][j];
			double magnitude = sqrt(x_2 + y_2);
      magnitudes[i][j] = magnitude;

			if(magnitude > max)
      {
        max = magnitude;
      }
      if(magnitude < min)
      {
        min = magnitude;
      } 
		}
	}
  double oldrange = max - min;
  double newmin=0;
  double newmax=255;
  double newrange = newmax - newmin;

  for( int i = 0; i < output.rows; i++ )
	{
		for( int j = 0; j < output.cols; j++ )
    {
      double scale = (magnitudes[i][j] - min) / oldrange;
      output.at<uchar>(i, j) = (newrange * scale) + newmin;
    }
  }
}

void SegmentByThreshold(Mat &input, double threshold, Mat &output) {
  output.create(input.size(), input.type());
	for (int i = 0; i < input.rows; i++)
  {
		for(int j = 0; j < input.cols; j++)
    {
      if (input.at<uchar>(i, j) >= threshold) {
        output.at<uchar>(i, j) = 255;
      } else {
        output.at<uchar>(i, j) = 0;
      }
		}
	}
}

void GradientDirection(double derivativesX[600][600], double derivativesY[600][600], Mat &output)
{
  double min = 999999, max = -999999, direction;
  double directions[550][550];
  for( int i = 0; i < output.rows; i++ )
	{
		for( int j = 0; j < output.cols; j++ )
    {
      direction = atan2(derivativesY[i][j] , derivativesX[i][j]);
      direction = direction * 180.0 / 3.14159265;
      directions[i][j] = direction;
      if(direction > max) max = direction;
      if(direction < min) min = direction;
		}
	}
  double oldrange = max - min;
  double newmin=0;
  double newmax=255;
  double newrange = newmax - newmin;

  for( int i = 0; i < output.rows; i++ )
	{
		for( int j = 0; j < output.cols; j++ )
    {
      double scale = (directions[i][j] - min) / oldrange;
      output.at<uchar>(i, j) = (newrange * scale) + newmin;
    }
  }
}

// void HoughSpace(Mat magnitude, Mat direction, int minRadius, int maxRaiud, float hSpace[600][600][150])
// {
//   //
// }