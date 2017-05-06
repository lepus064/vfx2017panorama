#include "brisk_descriptor.h"

using namespace std;
using namespace cv;

struct pt_pair{
    Point start;
    Point end;
    double dist;
};

void brisk_short(const cv::Mat& src,cv::KeyPoint kp,double rad){
    Rect rect(0, 0, 350, 350);
    Mat temp = src(rect);

    Mat temp2;
    GaussianBlur(temp,temp2, Size(0,0),5,0);
    // imshow("src",src);
    // imshow("temp", temp);
    // imshow("temp2", temp2);
    // waitKey(0);

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
    
    vector<pt_pair> ptp;

    for(int i=0; i<pts.size()-1;i++){
        for(int j=i+1; j < pts.size() ;j++){
            pt_pair temp_pr;
            temp_pr.start = pts[i];
            temp_pr.end = pts[j];
            double x_x = abs(pts[i].x - pts[j].x);
            double y_y = abs(pts[i].y - pts[j].y);
            temp_pr.dist = sqrt(x_x*x_x + y_y*y_y);
            ptp.push_back(temp_pr);
        }
    }


    RNG rng(12345);
    // for(const auto &i:ptp){
    //     if(i.dist < 9.75*rad/15){
    //         line(temp,i.start,i.end,Scalar(rng.uniform(0,205),rng.uniform(0,200),rng.uniform(0,200)));
    //     }
    // }

    for(const auto &i:ptp){
        if(i.dist > 13.67*rad/15){
            line(temp,i.start,i.end,Scalar(rng.uniform(0,205),rng.uniform(0,200),rng.uniform(0,200)));
        }
    }

    // for(const auto& pt:pts){
    //     circle(temp,pt,3,Scalar(255,255,255));
    // }

    cout << M_PI << endl;
    // circle(temp,center,3,Scalar(255,255,255));
    
    // resize(temp,temp,Size(),5,5);
    imshow("brisk",temp);
    waitKey(0);
}