#include "brisk_descriptor.h"

using namespace std;
using namespace cv;

void brisk_short(const cv::Mat& src,cv::KeyPoint kp,int rad){
    Rect rect(300, 300, 81, 81);
    Mat temp = src(rect);
    cout << temp.rows/2-0.5 << endl;
    Point2f center(temp.rows/2.0-0.5,temp.cols/2.0-0.5);
    // drawKeypoints(src,kp,temp);
    vector<Point> pts;
    vector<double> r;
    vector<int> r_num{10,14,15,20};

    r.push_back(double(rad)*4/15);
    r.push_back(double(rad)*7/15);
    r.push_back(double(rad)*10.5/15);
    r.push_back(double(rad));
    for(int i=0;i<4;i++){
        for(int j=0;j<r_num[i];j++){
            double x = center.x + r[i]*cos(2*M_PI*j/r_num[i]);
            double y = center.y + r[i]*sin(2*M_PI*j/r_num[i]);
            pts.push_back(Point2f(x,y));
        }
    }
    for(const auto& pt:pts){
        circle(temp,pt,3,Scalar(255,255,255));
    }

    cout << M_PI << endl;
    circle(temp,center,3,Scalar(255,255,255));
    imshow("brisk",temp);
    waitKey(0);
}