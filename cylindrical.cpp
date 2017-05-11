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
    double rows = left.rows/2.0 + right.rows/2.0 + abs(y2y);
    double overlap_x = (left.cols+right.cols)/2.0 - x2x;
    
    Mat res(rows,cols,CV_8UC3,Scalar(0,0,0));
    Mat temp_l(rows,1,CV_8UC3,Scalar(0,0,0));
    Mat temp_r(rows,1,CV_8UC3,Scalar(0,0,0));
    if(y2y < 0){
        left(Rect(0,0,left.cols - overlap_x,left.rows)).copyTo(res(Rect(0,-y2y,left.cols - overlap_x, left.rows)));
        right(Rect(overlap_x,0,right.cols - overlap_x,right.rows)).copyTo(res(Rect(left.cols,0,right.cols - overlap_x,right.rows)));
        int count = overlap_x;
        int edge = 5;
        double alpha,beta;
        for(int i = left.cols - overlap_x ;i < left.cols; i++){
            left(Rect(i,0,1,left.rows)).copyTo(temp_l(Rect(0,-y2y,1,left.rows)));
            right(Rect(overlap_x - count,0,1,right.rows)).copyTo(temp_r(Rect(0,0,1,right.rows)));
            if(count > overlap_x - edge)
                alpha = 1.0;
            else if(count <= edge)
                alpha = 0.0;
            else 
                alpha = (count-2*edge)/(overlap_x-2*edge);
            double beta = ( 1.0 - alpha );
            addWeighted( temp_l, alpha, temp_r, beta, 0.0, res(Rect(i,0,1,rows)));
            count--;
        }
    }
    else{
        left(Rect(0,0,left.cols - overlap_x,left.rows)).copyTo(res(Rect(0,0,left.cols - overlap_x, left.rows)));
        right(Rect(overlap_x,0,right.cols - overlap_x,right.rows)).copyTo(res(Rect(left.cols,y2y,right.cols - overlap_x,right.rows)));
        int count = overlap_x;
        int edge = 5;
        double alpha,beta;
        for(int i = left.cols - overlap_x ;i < left.cols; i++){
            left(Rect(i,0,1,left.rows)).copyTo(temp_l(Rect(0,0,1,left.rows)));
            right(Rect(overlap_x - count,0,1,right.rows)).copyTo(temp_r(Rect(0,y2y,1,right.rows)));
            if(count > overlap_x - edge)
                alpha = 1.0;
            else if(count <= edge)
                alpha = 0.0;
            else 
                alpha = (count-2*edge)/(overlap_x-2*edge);
            double beta = ( 1.0 - alpha );
            addWeighted( temp_l, alpha, temp_r, beta, 0.0, res(Rect(i,0,1,rows)));
            count--;
        }
    }
    return res;
}