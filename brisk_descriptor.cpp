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
    Mat sh,lo,un;
    sh = temp.clone();
    lo = temp.clone();
    un = temp.clone();
    Mat temp2;
    // GaussianBlur(temp,temp2, Size(0,0),0.5,0.5);
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

    r.push_back(double(rad)*2.9/10.8);
    r.push_back(double(rad)*4.9/10.8);
    r.push_back(double(rad)*7.4/10.8);
    r.push_back(double(rad));

    pts.push_back(center);

    for(int i=0;i<r.size();i++){
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
    int short_num = 0;
    int long_num = 0;
    for(const auto &i:ptp){
        if(i.dist < 9.75*rad/15.35){
            short_num++;
            line(sh,i.start,i.end,Scalar(rng.uniform(100,150),rng.uniform(100,150),rng.uniform(100,150)));
        }
        else if(i.dist > 13.67*rad/15.32){
            long_num++;
            line(lo,i.start,i.end,Scalar(rng.uniform(100,205),rng.uniform(100,200),rng.uniform(100,200)));
        }
        else{
            line(un,i.start,i.end,Scalar(rng.uniform(100,205),rng.uniform(100,200),rng.uniform(100,200)));
        }
    }
    cout << "pair:  " << ptp.size() << endl;
    cout << "short: " << short_num << endl;
    cout << "long:  " << long_num << endl;
    cout << "unuse: " << ptp.size() - long_num - short_num << endl;

    // for(const auto& pt:pts){
    //     circle(temp,pt,3,Scalar(255,255,255));
    // }

    cout << M_PI << endl;
    // circle(temp,center,3,Scalar(255,255,255));
    
    // resize(temp,temp,Size(),5,5);
    imshow("brisk_short",sh);
    imshow("brisk_long",lo);
    imshow("brisk_unuse",un);
    waitKey(0);
}

void brisk_compare(){
    Mat a,b;
    a = Mat::zeros(4,1,CV_8U);
    b = Mat::zeros(4,1,CV_8U);
    a.at<unsigned int>(3,0) = 255;
    b.at<unsigned int>(1,0) = 1;

    double dist_ham = norm(a,b,NORM_HAMMING);
    cout << a << endl;
    cout << dist_ham << endl;
}