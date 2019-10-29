/////////////////////////////////////////////////////////////////////////////
//
// COMS30121 - load.cpp
// TOPIC: load and display an image
//
// Getting-Started-File for OpenCV
// University of Bristol
//
/////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <opencv2/opencv.hpp>        //you may need to
#include <opencv2/imgproc/imgproc_c.h>

using namespace cv;           //make available OpenCV namespace

int main() {

  //declare a matrix container to hold an image
  Mat image;

  //load image from a file into the container
  image = imread("myimage.jpg", IMREAD_UNCHANGED);

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
