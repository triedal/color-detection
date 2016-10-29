#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <iostream>
using namespace cv;
using namespace std;
 
int main(int argc, char** argv) {

  VideoCapture video_stream(0);
 
  if (!video_stream.isOpened()) {
    cout << "cannot open camera";
    return -1;
  }

  int low_sat = 0;
  int high_sat = 255;
  int low_val = 0;
  int high_val = 255;

  namedWindow("Control", CV_WINDOW_AUTOSIZE);

  // Create trackbars to get Saturtion and Value
  cvCreateTrackbar("Low Saturtion", "Control", &low_sat, 255);
  cvCreateTrackbar("High Saturtion", "Control", &high_sat, 255);
  cvCreateTrackbar("Low Value", "Control", &low_val, 255);
  cvCreateTrackbar("High Value", "Control", &high_val, 255);
 
  while (true) {

    /*
     * B (blue)
     * G (green)
     * R (red)
     */ 

    Mat bgr_image;
    video_stream.read(bgr_image);

    Mat hsv_image;

    // Convert BGR image to HSV
    cvtColor(bgr_image, hsv_image, COLOR_BGR2HSV);

    /*
     * H (hue): color of the pixel
     * S (saturation): amount of white
     * V (value): amount of black
     */ 

    Mat lower_red_hue_range;
    Mat upper_red_hue_range;

    // Threshold the HSV image, keep only the red pixels
    inRange(hsv_image, Scalar(0, low_sat, low_val), Scalar(10, high_sat, high_val), lower_red_hue_range);
    inRange(hsv_image, Scalar(160, low_sat, low_val), Scalar(179, high_sat, high_val), upper_red_hue_range);

    Mat red_hue_image;
    addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);

    // Overlay text
    putText(red_hue_image, "Low Saturtion: " + to_string(low_sat), cvPoint(30, 30), FONT_HERSHEY_COMPLEX_SMALL,
      0.8, cvScalar(200, 200, 250), 1, CV_AA);
    putText(red_hue_image, "High Saturation: " + to_string(high_sat), cvPoint(30, 50), FONT_HERSHEY_COMPLEX_SMALL,
      0.8, cvScalar(200, 200, 250), 1, CV_AA);
    putText(red_hue_image, "Low Value: " + to_string(low_val), cvPoint(30, 70), FONT_HERSHEY_COMPLEX_SMALL,
      0.8, cvScalar(200, 200, 250), 1, CV_AA);
    putText(red_hue_image, "High Value: " + to_string(high_val), cvPoint(30, 90), FONT_HERSHEY_COMPLEX_SMALL,
      0.8, cvScalar(200, 200, 250), 1, CV_AA);


    imshow("Webcam Feed", bgr_image);
    imshow("Threshold Image", red_hue_image);

    if (waitKey(30) >= 0)
      break;
  }
  return 0;
}
