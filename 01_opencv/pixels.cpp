/////////////////////////////////////////////////////////////////////////////
//
// COMS30121 - pixels.cpp
// TOPIC: access and set individual pixels in an image
//
// Getting-Started-File for OpenCV
// University of Bristol
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <opencv2/opencv.hpp>

using namespace cv;

int main() {

  //create a black 256x256, 8bit, 3channel BGR image in a matrix container
  Mat image(256, 256, CV_8UC3, Scalar(0, 0, 0));

  //set pixels to create colour pattern
  for (int y = 0; y < image.rows; y++) //go through all rows (or scanlines)
    for (int x = 0; x < image.cols; x++) { //go through all columns
      image.at<Vec3b>(y, x)[0] = x; //set blue component
      image.at<Vec3b>(y, x)[1] = y; //set green component  
      image.at<Vec3b>(y, x)[2] = 255 - image.at<Vec3b>(y, x)[1]; //set red component
    }

  //construct a window for image display
  namedWindow("Display window", WINDOW_AUTOSIZE);

  //visualise the loaded image in the window
  imshow("Display window", image);

  //wait for a key press until returning from the program
  waitKey(0); 

  //free memory occupied by image 
  image.release();

  return 0;
}
