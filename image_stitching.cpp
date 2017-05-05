#include <opencv2/photo.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/core/matx.hpp"
#include "brisk_descriptor.h"
#include "cylindrical.h"
#include <dirent.h>

#include <algorithm>
#include <set>
#include <string>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <stdio.h>
#include <vector>
#include <thread>

using namespace cv;
using namespace std;

void loadExposureSeq(string, vector<Mat>&, vector<double>&);
double weight(int z);
int get_img_in_dir(string dir, vector<Mat> &images);
void create_octaves(const Mat&);
void fast_detect(const Mat& src, vector<KeyPoint> &kps, int v1, int v2);
int fast_score(const Mat &src, const vector<Point> &pts);
void reduce_point(Mat& src, int rad);


int main(int argc, char**argv){

    if(argc != 2){
        cout << "Usage: ./hdr_imaging ./Path_to_data" << endl;
        return 0;
    }


    // Some parameters
    vector<Mat> images; // Example 1024*768
    vector<double> times;
    vector<string> names;
    vector<Point> pts;

    get_img_in_dir(argv[1], images);
    // create_octaves(images[0]);


    vector<KeyPoint> kps;
    fast_detect(images[0],kps,9,16);
    Mat temp = images[0].clone();
    // Mat temp2;
    // drawKeypoints(temp,kps,temp2);
    // imshow("fastt",temp2);
    // waitKey(0);
    // brisk_short(images[0],kps[0],35);
    // for(auto i:kps){
    //     if(i.response > 2000)
    //         cout << i.pt.x << "," << i.pt.y << " " << i.response << endl;
    // }
    cylindrical(temp, kps,704.916);
    


    return 0;
}

double weight(int z){
    return z > 127 ? ((256-z)*1.0/128) : ((z+1)*1.0/128);
}

int get_img_in_dir(string dir, vector<Mat> &images){
    DIR *dp;
    struct dirent *dirp;
    if((dp = opendir(dir.c_str())) == NULL){
        cout << "Error" << endl;
        return 1;
    }
    set<string> names;

    while((dirp = readdir(dp)) != NULL){
        string jpg = string(dirp->d_name);
        if(jpg.find(".jpg") == jpg.size()-4||jpg.find(".JPG") == jpg.size()-4){
            names.insert(jpg);
        }
    }
    for(auto i:names){
        Mat img = imread(dir + "/" + i);
        images.push_back(img);
    }
    closedir(dp);
    return 0;
}

void fast_detect(const Mat& src, vector<KeyPoint> &kps, int v1, int v2){
    
    int edge = 0;
    vector<Point> pts;
    if(v2 == 16 && v1 == 9){
        edge = 3;
        pts.push_back(Point(3,0));
        pts.push_back(Point(4,0));
        pts.push_back(Point(5,1));
        pts.push_back(Point(6,2));
        pts.push_back(Point(6,3));
        pts.push_back(Point(6,4));
        pts.push_back(Point(5,5));
        pts.push_back(Point(4,6));
        pts.push_back(Point(3,6));
        pts.push_back(Point(2,6));
        pts.push_back(Point(1,5));
        pts.push_back(Point(0,4));
        pts.push_back(Point(0,3));
        pts.push_back(Point(0,2));
        pts.push_back(Point(1,1));
        pts.push_back(Point(2,0));
    }
    else if(v2 == 8 && v1 == 5){
        edge = 2;
    }
    auto temp = src;
    cvtColor(temp,temp,CV_RGB2GRAY);
    Mat fast_sc = Mat::zeros(temp.rows,temp.cols,CV_16U);

    // Rect rect(temp.cols-2*edge-1, temp.rows-2*edge-1, 2*edge+1, 2*edge+1);
    // Mat sub_m = temp(rect);
    // imshow("cc1",temp);
    // imshow("ccc",sub_m);
    // cout << edge << endl;

    // cout << src.cols << " " << src.rows << endl;
    // fast_sc.at<unsigned short>(193,257) = 255;
    
    // waitKey(0);
    for(int i = 0;i<temp.cols-2*edge;i++){
        for(int j = 0 ; j<temp.rows-2*edge ; j++){
            // fast_sc.at<uchar>(i+edge,j+edge) = 255;
            Rect rect(i, j, 2*edge+1, 2*edge+1);
            Mat sub_m = temp(rect);
            int center = sub_m.at<uchar>(edge,edge);
            int count = 0;
            if(abs(center - sub_m.at<uchar>(pts[0])) > 50)
                    count++;
            if(abs(center - sub_m.at<uchar>(pts[8])) > 50)
                    count++;
            if(count == 0)
                continue;
            if(abs(center - sub_m.at<uchar>(pts[4])) > 50)
                count++;
            if(abs(center - sub_m.at<uchar>(pts[12])) > 50)
                count++;
            if(count < 3)
                continue;
            if(abs(center - sub_m.at<uchar>(pts[1])) > 50)
                count++;
            if(abs(center - sub_m.at<uchar>(pts[2])) > 50)
                count++;
            if(abs(center - sub_m.at<uchar>(pts[3])) > 50)
                count++;
            if(abs(center - sub_m.at<uchar>(pts[5])) > 50)
                count++;
            if(abs(center - sub_m.at<uchar>(pts[6])) > 50)
                count++;
            if(abs(center - sub_m.at<uchar>(pts[7])) > 50)
                count++;
            if(abs(center - sub_m.at<uchar>(pts[9])) > 50)
                count++;
            if(abs(center - sub_m.at<uchar>(pts[10])) > 50)
                count++;
            if(abs(center - sub_m.at<uchar>(pts[11])) > 50)
                count++;
            if(abs(center - sub_m.at<uchar>(pts[13])) > 50)
                count++;
            if(abs(center - sub_m.at<uchar>(pts[14])) > 50)
                count++;
            if(abs(center - sub_m.at<uchar>(pts[15])) > 50)
                count++;
            if(count >= 9){
                // cout << "(" << i+edge << "," << j+edge << ")" << " ";
                // cout << fast_score(sub_m,pts) << endl;
                // fast_sc.at<unsigned short>(i+edge, j+edge) = fast_score(sub_m,pts);
                fast_sc.at<unsigned short>(j+edge, i+edge) = fast_score(sub_m,pts);
                
            }
        }
    }
    
    reduce_point(fast_sc,5);
    Mat fast_mat(temp.rows,temp.cols,CV_8U,Scalar(0));
    kps.clear();

    for(int i = 0;i<fast_sc.cols;i++){
        for(int j = 0 ; j<fast_sc.rows ; j++){
            if(fast_sc.at<unsigned short>(j,i) > 0){
                // cout << i << "," << j << " " << fast_sc.at<unsigned short>(j,i) << endl;
                // fast_mat.at<uchar>(j,i) = 255;
                KeyPoint kp(i, j, edge+0.5);
                kp.response = fast_sc.at<unsigned short>(j,i);
                kps.push_back(kp);
            }
        }
    }
    

    // imshow("origin",temp);
    // imshow("fast",fast_mat);
    // waitKey(0);

}

int fast_score(const Mat &src, const vector<Point> &pts){
    // Point center((src.cols+1)/2,(src.rows+1)/2);
    int center = src.at<uchar>((src.cols+1)/2,(src.rows+1)/2);
    int sc = 0;
    for(auto &i:pts){
        sc += abs(center - src.at<uchar>(i));
    }
    return sc;
}

void reduce_point(Mat& src, int rad){
    
    vector<Point> temp_pts;
    for(int k=0;k<rad;k++){
        for(int l=0;l<rad;l++){
            temp_pts.push_back(Point(k,l));
        }
    }
    

    for(int i=0;i<=src.cols-rad;i++){
        for(int j=0;j<=src.rows-rad;j++){

            int max = 0;
            for(int k=0;k<rad;k++){
                for(int l=0;l<rad;l++){
                    if(src.at<unsigned short>(j+l,i+k) > max){
                        max = src.at<unsigned short>(j+l,i+k);
                    }
                }
            }
            if(max > 0){
                for(int k=0;k<rad;k++){
                    for(int l=0;l<rad;l++){
                        if(src.at<unsigned short>(j+l,i+k) < max){
                            src.at<unsigned short>(j+l,i+k) = 0;
                        }
                    }
                } 
            }
            // int a;
            // cin >> a;
           
            //         if(sub_m.at<uchar>(k,l)>center){
            //             src.at<unsigned short>(i+(rad-1)/2,j+(rad-1)/2) = 0;
            //             // break;
            //         }
            //     }
            // }
        }
    }
    // src.at<unsigned short>(380,231) = 0;
    // imshow("re",src);
    // waitKey(0);
}

void create_octaves(const Mat &src){
    vector<Mat> c(4);
    vector<Mat> d(4);
    vector<KeyPoint> kp;
    c[0] = src;
    // resize(src,c[0],Size(),0.5,0.5);
    resize(src,d[0],Size(),1.0/1.5,1.0/1.5);

    for(int i = 0;i<3;i++){
        resize(c[i],c[i+1],Size(),0.5,0.5);
        resize(d[i],d[i+1],Size(),0.5,0.5);
    }
    // for(auto i:c){
    //     imshow("c",i);
    //     waitKey(0);
    // }

    FAST(c[0],kp,30,true,FastFeatureDetector::TYPE_9_16);
    drawKeypoints(c[0],kp,c[0]);
    
    // imshow("origin",src);
    imshow("test",c[0]);
    waitKey(0);
}