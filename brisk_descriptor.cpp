#include "brisk_descriptor.h"

using namespace std;
using namespace cv;

void brisk_short(const cv::Mat& src,cv::KeyPoint kp){
    Rect rect(300, 300, 40, 40);
    Mat temp = src(rect);
    cout << kp.size << endl;
    // drawKeypoints(src,kp,temp);
    circle(temp,Point2f(19.5,19.5),3,Scalar(255,255,255));
    imshow("brisk",temp);
    waitKey(0);
}