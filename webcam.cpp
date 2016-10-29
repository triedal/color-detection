/*
* File:   main.cpp
* Author: sagar
*
* Created on 10 September, 2012, 7:48 PM
*/
 
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
using namespace cv;
using namespace std;
 
int main() {
  VideoCapture vidStream(0);   //0 is the id of video device.0 if you have only one camera.
 
  if (!vidStream.isOpened()) { //check if video device has been initialised
    cout << "cannot open camera";
    return -1;
  }
 
  //unconditional loop
  while (true) {

    /*
     * B (blue)
     * G (green)
     * R (red)
     */ 

    Mat bgr_image;
    vidStream.read(bgr_image);

    Mat hsv_image;

    // Convert BGR image to HSV
    cvtColor(bgr_image, hsv_image, COLOR_BGR2HSV);

    /*
     * H (hue): color of the pixel
     * S (saturation): amount of white
     * V (value): amount of black
     */ 

    // Threshold the HSV image, keep only the red pixels
    Mat lower_red_hue_range;
    Mat upper_red_hue_range;

    inRange(hsv_image, Scalar(0, 100, 100), Scalar(100, 255, 255), lower_red_hue_range);
    inRange(hsv_image, Scalar(160, 100, 100), Scalar(179, 255, 255), upper_red_hue_range);

    Mat red_hue_image;
    addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);



    imshow("Webcam Feed", bgr_image);
    imshow("Threshold Image", red_hue_image);

    if (waitKey(30) >= 0)
      break;
  }
  return 0;
}
