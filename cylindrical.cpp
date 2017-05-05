#include "cylindrical.h"

using namespace std;
using namespace cv;

void cylindrical(Mat& src, vector<cv::KeyPoint> &kps, double f){
    Mat result_mat(src.rows,src.cols,CV_8UC3, Scalar(0,0,0));
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
    src = result_mat;
    // imshow("ee1",result_mat);
    // drawKeypoints(result_mat,kps,result_mat);
    // imshow("Kp",result_mat);
    // waitKey(0);
}

Mat cylindrical_merge(const Mat& left, const Mat& right, double x2x, double y2y, double angle = 0){
    int cols = left.cols/2.0 + right.cols/2.0 + x2x;
    int rows = left.rows/2.0 + right.rows/2.0 + y2y;
    double overlap_x = (left.cols+right.cols)/2.0 - x2x;
    
    Mat res(rows,cols,CV_8UC3);
    if(y2y < 0){

    }
    else{
        left(Rect(0,0,left.cols - overlap_x,left.rows)).copyTo(res(Rect(0,0,left.cols - overlap_x, right.rows)));
        right(Rect(overlap_x,0,right.cols - overlap_x,right.rows)).copyTo(res(Rect(left.cols,y2y,right.cols - overlap_x,right.rows)));
        // for(int i = 0;i < left.cols; i++){
        //     for(int j = 0;j<left.rows; j++){
        //         if(i > left.cols - ((left.cols+right.cols)/2.0 - x2x)){
        //             res.at<>left.at<Vec3b>(j,i);
        //         }
        //     }
        // }
    }
    return res;
}