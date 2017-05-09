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
#include <deque>
#include <thread>

using namespace cv;
using namespace std;

void loadExposureSeq(string, vector<Mat>&, vector<double>&);
double weight(int z);
int get_img_in_dir(string dir, vector<Mat> &images);
void create_octaves(const Mat &src,vector<Mat>& c_octaves, vector<Mat>& d_octaves);
void fast_detect(const Mat& src, vector<KeyPoint> &kps, int v1, int v2);
int fast_score(const Mat &src, const vector<Point> &pts);
void reduce_point(Mat& src, int rad);
void keypoint_real_post(vector<KeyPoint>& kps, double f);
vector<KeyPoint> reduce_pt_from_octaves(const Mat& src,deque<vector<KeyPoint> > all_kps);
vector<KeyPoint> get_fast_keypoint(const Mat& src);
vector<double> get_subpixel_and_octave(vector<KeyPoint>& kps, const Mat& src);
double get_octave_size(int octave);

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
    vector<vector<KeyPoint> > all_kps;
    vector<vector<double> > response_octave;
    vector<vector<Mat> > brisk_d;

    get_img_in_dir(argv[1], images);
    // create_octaves(images[0]);

    vector<vector<vector<KeyPoint> > > kpss(images.size());
    // Mat temp_mat = .clone();
    all_kps.push_back(get_fast_keypoint(images[0]));
    // get_subpixel_and_octave(all_kps[0],images[0]);
    brisk_short(images[0],all_kps[0][0],1);
    // brisk_compare();

    //kps[images][octaves][keypoints]
    // kps[0].resize(9);
    // for(int i=0;i<images.size();i++){
    //     FAST(images[i],kps[i],40,true,FastFeatureDetector::TYPE_9_16);
    // }

    // fast_detect(images[0],kps[0][0],5,8);

    

    // brisk_short(images[0],kps[0][0],100);
    // brisk_compare();
    // for(auto i:kps){
    //     if(i.response > 2000)
    //         cout << i.pt.x << "," << i.pt.y << " " << i.response << endl;
    // }


    /* cylindrical */
    
    // cylindrical(temp, kps,704.916);
    // cylindrical(temp2,kps,706.286);
    // imshow("right",temp);
    // imshow("left",temp2);
    // imshow("cylindrical", cylindrical_merge(temp2,temp,244,5,0));
    // waitKey(0);

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
        edge = 1;
        pts.push_back(Point(1,0));
        pts.push_back(Point(2,0));
        pts.push_back(Point(2,1));
        pts.push_back(Point(2,2));
        pts.push_back(Point(1,2));
        pts.push_back(Point(0,2));
        pts.push_back(Point(0,1));
        pts.push_back(Point(0,0));
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
            if(v2 == 16 && v1 == 9){
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
            }
            else if(v2 == 8 && v1 == 5){
                if(abs(center - sub_m.at<uchar>(pts[0])) > 50)
                    count++;
                if(abs(center - sub_m.at<uchar>(pts[4])) > 50)
                    count++;
                if(count == 0)
                    continue;
                if(abs(center - sub_m.at<uchar>(pts[2])) > 50)
                    count++;
                if(abs(center - sub_m.at<uchar>(pts[6])) > 50)
                    count++;
                if(count < 3)
                    continue;
                if(abs(center - sub_m.at<uchar>(pts[1])) > 50)
                    count++;
                if(abs(center - sub_m.at<uchar>(pts[3])) > 50)
                    count++;
                if(abs(center - sub_m.at<uchar>(pts[5])) > 50)
                    count++;
                if(abs(center - sub_m.at<uchar>(pts[7])) > 50)
                    count++;
            }
            
            if(count >= v1){
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
                fast_mat.at<uchar>(j,i) = 255;
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

void create_octaves(const Mat &src,vector<Mat>& c_octaves, vector<Mat>& d_octaves){
    vector<Mat> c(4);
    deque<Mat> d(4);
    vector<KeyPoint> kp;
    c[0] = src;
    // resize(src,c[0],Size(),0.5,0.5);
    resize(src,d[0],Size(),1.0/1.5,1.0/1.5);

    for(int i = 0;i<3;i++){
        resize(c[i],c[i+1],Size(),0.5,0.5);
        resize(d[i],d[i+1],Size(),0.5,0.5);
    }
    Mat d0(src);
    d.push_front(d0);
    c_octaves.assign(c.begin(),c.end());
    d_octaves.assign(d.begin(),d.end());
    // FAST(c[0],kp,30,true,FastFeatureDetector::TYPE_9_16);
    // drawKeypoints(c[0],kp,c[0]);
    
    // imshow("origin",src);
    // imshow("test",c[0]);
    // waitKey(0);
}
void keypoint_real_post(vector<KeyPoint>& kps, double f){
    for(auto& i:kps){
        i.pt.x *= f;
        i.pt.y *= f;
    }
}
vector<KeyPoint> reduce_pt_from_octaves(const Mat& src,deque<vector<KeyPoint> > all_kps){
    vector<KeyPoint> res;
    
    for(int i = 1;i<all_kps.size()-1;i++){
        Mat octave0 = Mat::zeros(src.rows,src.cols,CV_16U);
        // Mat octave1 = Mat::zeros(src.rows,src.cols,CV_16U);
        Mat octave2 = Mat::zeros(src.rows,src.cols,CV_16U);

        for(const auto& j:all_kps[i-1]){
            octave0.at<unsigned short>(j.pt) = j.response;
        }
        for(const auto& j:all_kps[i+1]){
            octave2.at<unsigned short>(j.pt) = j.response;
        }
        for(auto j:all_kps[i]){
            bool true_kp = true;
            int x0 = j.pt.x-1;
            int y0 = j.pt.y-1;
            int max_res = 0;
            for(int k = x0;k<x0+2;k++){
                for(int l = y0;l<y0+2;l++){
                    if(max_res < octave0.at<unsigned short>(l,k))
                        max_res = octave0.at<unsigned short>(l,k);
                    if(max_res < octave2.at<unsigned short>(l,k))
                        max_res = octave2.at<unsigned short>(l,k);
                }
            }
            if(max_res < j.response){
                j.octave = i-1;
                // if(i%2 == 0)
                //     j.octave = 2.0/3.0/pow(2,(i/2)-1);
                // else
                //     j.octave = 1.0/pow(2,(i-1)/2);
                res.push_back(j);
            }
        }

    }
    return res;
}

vector<KeyPoint> get_fast_keypoint(const Mat& src){
    
    //create octaves
    Mat temp = src.clone();
    vector<Mat> c_temp; //  0 ~ 3
    vector<Mat> d_temp; // -1 ~ 3
    create_octaves(temp,c_temp,d_temp);

    //keypoint from octaves
    deque<vector<KeyPoint> > kps1; 
    for(int i = 0;i<4;i++){
        vector<KeyPoint> temp_kps1;
        vector<KeyPoint> temp_kps2;
        fast_detect(c_temp[i],temp_kps1,9,16);
        fast_detect(d_temp[i+1],temp_kps2,9,16);
        // drawKeypoints(c_temp[i],temp_kps1,c_temp[i]);
        // drawKeypoints(d_temp[i+1],temp_kps2,d_temp[i+1]);
        keypoint_real_post(temp_kps1,pow(2,i));
        keypoint_real_post(temp_kps2,1.5*pow(2,i));
        // imshow("c",c_temp[i]);
        // imshow("d",d_temp[i+1]);
        kps1.push_back(temp_kps1);
        kps1.push_back(temp_kps2);
        // waitKey(0);
    }
    vector<KeyPoint> temp_kps0; //d0
    fast_detect(temp,temp_kps0,5,8);
    kps1.push_front(temp_kps0);

    vector<KeyPoint> res = reduce_pt_from_octaves(temp,kps1);

    /* Draw all keypoints */
    // drawKeypoints(temp,res,temp);
    // imshow("all point reduce",temp);
    // waitKey(0);
    return res;
}

//Not done
vector<double> get_subpixel_and_octave(vector<KeyPoint>& kps, const Mat& src){
    // for(auto i:kps){
    //     get_octave_size(i.octave);
    // }
        // if(i.octave > 0)
        //     cout << "Response: " << i.response << ", octave:" << i.octave << endl;
    // cout << kps.size() << endl;
    vector<double> res;
    return res;
}

double get_octave_size(int octave){
    /* 
    octave number
    0 -> c0 -> 1
    1 -> d0 -> 2/3
    2 -> c1 -> 1/2
    3 -> d1 -> 1/3
    4 -> c2 -> 1/4
    5 -> d2 -> 1/6
    6 -> c3 -> 1/8
    7 -> d3 -> 1/12
    */
    if(octave%2 == 0)
        return 1.0/pow(2,octave/2); 
    else
        return 2.0/3.0/pow(2,(octave-1)/2);
}