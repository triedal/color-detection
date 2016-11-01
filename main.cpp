#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <string>
#include <iostream>

using namespace cv;
using namespace std;

void createController(const char *window_name, int &low_sat, int &high_sat, int &low_val, int &high_val) {
  namedWindow(window_name, CV_WINDOW_AUTOSIZE);

  // Create trackbars to get Saturtion and Value
  cvCreateTrackbar("Low Saturtion", window_name, &low_sat, 255);
  cvCreateTrackbar("High Saturtion", window_name, &high_sat, 255);
  cvCreateTrackbar("Low Value", window_name, &low_val, 255);
  cvCreateTrackbar("High Value", window_name, &high_val, 255);
}
 
int main(int argc, char** argv) {

  VideoCapture video_stream(0);
 
  if (!video_stream.isOpened()) {
    cout << "cannot open camera";
    return -1;
  }

  int low_sat = 167;
  int high_sat = 255;
  int low_val = 33;
  int high_val = 255;

  createController("Controls", low_sat, high_sat, low_val, high_val);
 
  while (true) {

    /*
     * B (blue)
     * G (green)
     * R (red)
     */ 

    Mat bgr_image,
        hsv_image,
        lower_red_hue_range,
        upper_red_hue_range,
        red_hue_image,
        red_hue_image_binary;

    video_stream.read(bgr_image);

    // Convert BGR image to HSV
    cvtColor(bgr_image, hsv_image, COLOR_BGR2HSV);

    /*
     * H (hue): color of the pixel
     * S (saturation): amount of white
     * V (value): amount of black
     */ 

    // Threshold the HSV image, keep only the red pixels
    inRange(hsv_image, Scalar(0, low_sat, low_val), Scalar(10, high_sat, high_val), lower_red_hue_range);
    inRange(hsv_image, Scalar(160, low_sat, low_val), Scalar(179, high_sat, high_val), upper_red_hue_range);

    addWeighted(lower_red_hue_range, 1.0, upper_red_hue_range, 1.0, 0.0, red_hue_image);

    // Morphological opening (remove small noise objects)
    erode(red_hue_image, red_hue_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    erode(red_hue_image, red_hue_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    dilate(red_hue_image, red_hue_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    dilate(red_hue_image, red_hue_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

    // Morphological closing (fill in small holes in target object)
    dilate(red_hue_image, red_hue_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
    erode(red_hue_image, red_hue_image, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

    threshold(red_hue_image, red_hue_image_binary, 150, 255, CV_THRESH_BINARY);

    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    int largest_area, largest_idx = 0;

    findContours(red_hue_image_binary, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);

    vector<Rect> boundRect( contours.size() );
    vector<vector<Point> > contours_poly( contours.size() );

    for (int i = 0; i < contours.size(); i++) {
      double area = contourArea(contours[i]);

      if (area > largest_area) {
        largest_area = area;
        largest_idx = i;
      }
    } 

     for ( int i = 0; i < contours.size(); i++ ) { 
         approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
         boundRect[i] = boundingRect( Mat(contours_poly[i]) );
     }

     for ( int i = 0; i< contours.size(); i++ ) {
         Scalar color = Scalar(0, 255, 0);
         rectangle( bgr_image, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );          
     }

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
