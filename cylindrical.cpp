#include "cylindrical.h"

using namespace std;
using namespace cv;

void cylindrical(Mat& src, vector<cv::KeyPoint> &kps, double f){
    Mat result_mat(src.rows,src.cols,CV_8UC3);
    Point2f center((src.cols-1)/2.0,(src.rows-1)/2.0);
    for(int i = 0;i<src.cols;i++){
        for(int j = 0 ;j<src.rows;j++){
            double x = f*atan((i-center.x)/f);
            double y = f*(j-center.y)/sqrt((i-center.x)*(i-center.x)+f*f);
            x = center.x + x;
            y = center.y + y;
            Point2f pt(x,y);
            result_mat.at<Vec3b>(pt) = src.at<Vec3b>(j,i);
            // cin >> x;
        }
    }

    for(auto &i:kps){
        i.pt.x = center.x + f*atan((i.pt.x-center.x)/f);
        i.pt.y = center.y + f*(i.pt.y-center.y)/sqrt((i.pt.x-center.x)*(i.pt.x-center.x)+f*f);
    }

    imshow("ee1",result_mat);
    drawKeypoints(result_mat,kps,result_mat);
    imshow("Kp",result_mat);
    waitKey(0);
}