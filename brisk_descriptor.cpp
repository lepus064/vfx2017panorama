#include "brisk_descriptor.h"

using namespace std;
using namespace cv;

void brisk_short(const cv::Mat& src,cv::KeyPoint kp,double oc_size){
    Rect rect(0, 0, 350, 350);
    Mat temp = src(rect);
    cvtColor(temp,temp,CV_RGB2GRAY);
    Mat sh,lo,un;
    sh = temp.clone();
    lo = temp.clone();
    un = temp.clone();
    
    vector<Mat> blurred_img(5);
    double blur_f = 0.5*oc_size;
    for(int i = 0;i<5;i++){
        Mat temp_blur;
        double sigma = (i+1)/5.0*blur_f;
        GaussianBlur(temp,temp_blur, Size(0,0),sigma,sigma);
        blurred_img[i] = temp_blur;
    }
    // for(auto i:blurred_img){
    //     imshow("b",i);
    //     waitKey(0);
    // }
        
    // GaussianBlur(temp,temp2, Size(0,0),0.5,0.5);
    // imshow("src",src);
    // imshow("temp", temp);
    // imshow("temp2", temp2);
    // waitKey(0);

    // cout << temp.rows/2-0.5 << endl;
    Point2f center(temp.rows/2.0-0.5,temp.cols/2.0-0.5);

    // drawKeypoints(src,kp,temp);
    // vector<Point2f> pts;
    vector<pt_value> pvs;
    vector<double> r(4);
    vector<int> r_num{10,14,15,20}; 
    
    double f = 0.85*oc_size;
    r[0] = 2.9*f;
    r[1] = 4.9*f;
    r[2] = 7.4*f;
    r[3] = 10.8*f;

    
    pt_value pv;
    pv.point = center;
    pv.I = blurred_img[0].at<uchar>(center);
    // pts.push_back(center);
    pvs.push_back(pv);

    for(int i=0;i<r.size();i++){
        for(int j=0;j<r_num[i];j++){
            double x = center.x + r[i]*cos(2*M_PI*j/double(r_num[i]));
            double y = center.y + r[i]*sin(2*M_PI*j/double(r_num[i]));
            // pts.push_back(Point2f(x,y));
            pt_value temp_pv;
            temp_pv.point = Point2f(x,y);
            temp_pv.I = blurred_img[i+1].at<uchar>(temp_pv.point);
            pvs.push_back(temp_pv);
        }
    }
    
    vector<pt_pair> long_pair;
    vector<pt_pair> short_pair;
    vector<pt_pair> unuse_pair;

    for(int i=0; i<pvs.size()-1;i++){
        for(int j=i+1; j < pvs.size() ;j++){
            pt_pair temp_pr;
            temp_pr.start = pvs[i].point;
            temp_pr.end = pvs[j].point;
            double x_x = abs(pvs[i].point.x - pvs[j].point.x);
            double y_y = abs(pvs[i].point.y - pvs[j].point.y);
            temp_pr.dist = sqrt(x_x*x_x + y_y*y_y);
            temp_pr.S_I = pvs[i].I;
            temp_pr.E_I = pvs[j].I;
            if(temp_pr.dist < 5.85*oc_size)
                short_pair.push_back(temp_pr);
            else if(temp_pr.dist > 8.2*oc_size)
                long_pair.push_back(temp_pr);
        }
    }
    double gx = 0;
    double gy = 0;
    for(const auto& i:long_pair){
        double E = (i.E_I - i.S_I);
        double dx = (i.end.x - i.start.x)*E/i.dist/i.dist;
        double dy = (i.end.y - i.start.y)*E/i.dist/i.dist;
        gx += dx;
        gy += dy;
    }
    gx /= 870;
    gy /= 870;
    cout << gx << endl;
    cout << gy << endl;
    cout << sqrt(gx*gx+gy*gy) << endl;
    cout << atan2(gy,gx) << endl;
    cout << atan2(0,-1) << endl;
    RNG rng(12345);
    int short_num = short_pair.size();
    int long_num = long_pair.size();

    cout << "short: " << short_num << endl;
    cout << "long:  " << long_num << endl;
    // cout << "unuse: " << ptp.size() - long_num - short_num << endl;

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