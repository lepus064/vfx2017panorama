#include "cylindrical.h"

using namespace std;
using namespace cv;

void cylindrical(Mat& src, vector<cv::KeyPoint> kps, double f){
    Mat result_mat(src.rows,src.cols,CV_8UC3);
    Point2f center((src.rows-1)/2.0,(src.cols-1)/2.0);
    imshow("ee",result_mat);
    
    waitKey(0);
}