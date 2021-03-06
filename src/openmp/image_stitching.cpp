#include <opencv2/photo.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/core/matx.hpp"
#include "brisk_descriptor.h"
#include "cylindrical.h" 
#include <dirent.h>
#include <omp.h>

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
#include <map>
#include <thread>
#include <future>
#include <cstdlib>

using namespace cv;
using namespace std;

struct kp_pair{
    KeyPoint kp1;
    KeyPoint kp2;
    int kp1_ID;
    int kp2_ID;
};

void loadExposureSeq(string, vector<Mat>&, vector<double>&);
double weight(int z);
int get_img_in_dir(string dir, vector<Mat> &images, map<string,double> mf,vector<double>& f_f);
void create_octaves(const Mat &src,vector<Mat>& c_octaves, vector<Mat>& d_octaves);
void fast_detect(const Mat& src, vector<KeyPoint> &kps, int v1, int v2);
int fast_score(const Mat &src, const vector<Point> &pts);
void reduce_point(Mat& src, int rad);
void keypoint_real_post(vector<KeyPoint>& kps, double f);
vector<KeyPoint> reduce_pt_from_octaves(const Mat& src,deque<vector<KeyPoint> > all_kps);
vector<KeyPoint> get_fast_keypoint(const Mat& src);
vector<double> get_subpixel_and_octave(vector<KeyPoint>& kps, const Mat& src);
double get_octave_size(int octave);
int Ransac(vector<kp_pair>& kpp, const int& number, int time, double &dx, double &dy);
int myrandom (int i) { return std::rand()%i;}
map<string,double> get_f(string s);
void panorama(const vector<Mat> &cy_Mat, const vector<pair<double,double> > &dxdy);
void get_all_kps(vector<vector<KeyPoint> > &all_kps, const vector<Mat> &images);

int main(int argc, char**argv){

    if(argc != 3){
        cout << "Usage: ./hdr_imaging ./Path_to_data ./Path_to_pano.txt" << endl;
        return 0;
    }
    

    // All parameters
    vector<Mat> images; // Example 1024*768
    vector<double> times;
    // vector<string> names;
    // vector<Point> pts;

    // All variables
    vector<vector<KeyPoint> > all_kps;
    vector<vector<Mat> > brisk_d; //[img][brisk_descriptor for keypoint]
    vector<double> factor_f; // TODO
    vector<Mat> cy_Mat; // cylindrical images
    vector<pair<double,double> > dx_dy_;

    int thread_num = 5;


    get_img_in_dir(argv[1], images, get_f(argv[2]), factor_f);
    if(images.size() != factor_f.size()){
        cout << "pano.txt is not correct." << endl;
        return 0;
    }

    brisk_d.resize(images.size());
    
    cout << endl << "Detecting all feature points." << endl << endl;
    


    // get_all_kps(all_kps,images);
    // cout << all_kps.size() << endl;
    // for(const auto& i:images){
    //     all_kps.push_back(get_fast_keypoint(i));
    // }

    // return 0;
    // all_kps.push_back(get_fast_keypoint(images[0]));
    // all_kps.push_back(get_fast_keypoint(images[1]));
    // get_subpixel_and_octave(all_kps[0],images[0]);
    int img_ = images.size(); // 14 is fine
    all_kps.resize(img_);

    
    
    for(int j = 0;j < img_;j++){
    // for(int j = 0;j<4;j++){
        // all_kps.push_back(get_fast_keypoint(images[j]));
        all_kps[j] = get_fast_keypoint(images[j]);
        
        cout << "Calculating image" << j << " feature descriptors." << endl;
        
        brisk_d[j].resize(all_kps[j].size());
        
        #pragma omp parallel for
        for(int i = 0;i < all_kps[j].size();i++)
            brisk_d[j][i] = brisk_short(images[j],all_kps[j][i],get_octave_size(all_kps[j][i].octave));
        // for(auto i : all_kps[j]){
        //     brisk_d[j].push_back(brisk_short(images[j],i,get_octave_size(i.octave)));
        // }
    }

    int max_hamming_distance = 120;
    int ransac_times = 1500;
    bool left2right = true;

    cout << endl;
    Mat r3;

    
    for(int m = 0; m < img_-1 ; m++){
        cout << "Start to merge image " << m << " and " << m+1 << "." << endl;
        Mat r1,r2;
        r1 = images[m].clone();
        r2 = images[m+1].clone();

        RNG color_bgr;
        vector<kp_pair> true_kp;

        cylindrical(r1,all_kps[m],factor_f[m]);
        cylindrical(r2,all_kps[m+1],factor_f[m+1]);

        if(m == 0)
            cy_Mat.push_back(r1);
        cy_Mat.push_back(r2);
        
        #pragma omp parallel for
        for(int i = 0; i < brisk_d[m].size();i++){
            int a = key_pair(brisk_d[m][i],brisk_d[m+1],max_hamming_distance);
            if(a != -1){
                int B = color_bgr.uniform(0,255);
                int G = color_bgr.uniform(0,255);
                int R = color_bgr.uniform(0,255);
                kp_pair temp_kpp;
                temp_kpp.kp1 = all_kps[m][i];
                temp_kpp.kp2 = all_kps[m+1][a];
                temp_kpp.kp1_ID = i;
                temp_kpp.kp2_ID = a;
                true_kp.push_back(temp_kpp);

                // circle(r1,temp_kpp.kp1.pt,3,Scalar(B,G,R));
                // circle(r2,temp_kpp.kp2.pt,3,Scalar(B,G,R));
            }
        }

        double dx,dy;
        Ransac(true_kp,4,ransac_times,dx,dy);
        dx_dy_.push_back(pair<double,double>(dx,dy));

        // imshow("r1",r1);
        // imshow("r2",r2);
        // imshow("r3",r3);
        // waitKey(0);
    }

    // r3 = cylindrical_merge(cy_Mat[1],cy_Mat[0],-dx_dy_[0].first,-dx_dy_[0].second,0);
    // imshow("r3",r3);
    // waitKey(0);
    // r3 = cylindrical_merge(cy_Mat[2],r3,400,-dx_dy_[1].second,0);
    // imshow("r3",r3);
    // waitKey(0);

    panorama(cy_Mat,dx_dy_);
    
    return 0;
}

double weight(int z){
    return z > 127 ? ((256-z)*1.0/128) : ((z+1)*1.0/128);
}

int get_img_in_dir(string dir, vector<Mat> &images, map<string,double> mf, vector<double>& f_f){
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
            if(jpg[0] != '.')
                names.insert(jpg);
        }
    }
    for(auto i:names){
        Mat img = imread(dir + "/" + i);
        images.push_back(img);
        f_f.push_back(mf[i]);
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

int Ransac(vector<kp_pair>& kpp,const int& number, int times, double &dx, double &dy){
    vector<kp_pair> result_kpp;
    srand ( unsigned ( std::time(0) ) );
    double bias = 3.0;
    int s = kpp.size();
    double final_x = 0;
    double final_y = 0;
    int vote = 0;

    
    for(int i = 0; i < times; i++){
        double x_x = 0;
        double y_y = 0;
        int temp_vote = 0;
        random_shuffle ( kpp.begin(), kpp.end(), myrandom);
        for(int j = 0; j < number ; j++){
            x_x += kpp[j].kp1.pt.x - kpp[j].kp2.pt.x;
            y_y += kpp[j].kp1.pt.y - kpp[j].kp2.pt.y;
        }
        x_x /= number;
        y_y /= number;
        if(abs(y_y/x_x) > 0.25)
            continue;
        

        for(const auto& j:kpp){
            double x = j.kp1.pt.x - j.kp2.pt.x;
            double y = j.kp1.pt.y - j.kp2.pt.y;
            if((x < x_x+bias) && (x > x_x-bias)){
                if((y < y_y+bias) && (y > y_y-bias))
                    temp_vote++;
            }
        }
        if(temp_vote > vote){
            vote = temp_vote;
            final_x = x_x;
            final_y = y_y;
        }
    }

    double result_x = 0;
    double result_y = 0;


    for(const auto &i:kpp){
        double x = i.kp1.pt.x - i.kp2.pt.x;
        double y = i.kp1.pt.y - i.kp2.pt.y;
        if((x < final_x+bias) && (x > final_x-bias)){
            if((y < final_y+bias) && (y > final_y-bias)){
                result_x += x;
                result_y += y;
                result_kpp.push_back(i);
            }
        }
    }
    result_x /= result_kpp.size();
    result_y /= result_kpp.size();
    kpp = result_kpp;

    dx = result_x;
    dy = result_y;
    return vote;
}

map<string,double> get_f(string s){
    map<string,double> map_f;
    fstream fs(s);
    string temp;
    string file_name = "\\";
    while(fs >> temp){
        string name = "";
        double f = 0.0;
        auto it = find_end(temp.cbegin(),temp.cend(),file_name.cbegin(),file_name.cend());
        it++;
        while(it != temp.cend()){
            name += *it;
            it++;
        }
        for(int i = 0; i < 20;i++)
            fs >> temp;
        fs >> f;
        // cout << name << " , " << f << endl;
        map_f[name] = f;
        // break;
    }
    
    return map_f;
}

void get_all_kps(vector<vector<KeyPoint> > &all_kps, const vector<Mat> &images){

    int times = images.size()/2;
    int remain = images.size()%2;

}

void panorama(const vector<Mat> &cy_Mat, const vector<pair<double,double> > &dxdy){
    cout << "Generating panorama !!" << endl;
    Mat result_mat = cy_Mat[0].clone();


    double y_bias = 0;
    double x_bias = 0;
    
    #pragma omp parallel for
    for(int i =0;i<dxdy.size();i++){
        x_bias += abs(dxdy[i].first);
        y_bias += dxdy[i].second;
    }
        

    // for(const auto &i:dxdy)
    //     y_bias += i.second;
    y_bias /= x_bias;
    // cout << y_bias << endl;
    // for(int i = 0; i < dxdy.size();i++){
    
    for(int i = 0; i < dxdy.size();i++){
        double temp_dx = 0;
        double temp_dy = 0;
        if(dxdy[i].first > 0){
            temp_dx = (result_mat.cols - cy_Mat[i+1].cols)/2.0 + dxdy[i].first;
            // temp_dy += (dxdy[i].second - y_bias);
            result_mat = cylindrical_merge(result_mat,cy_Mat[i+1],temp_dx,(dxdy[i].second - y_bias*dxdy[i].first),0);
        }
        else{
            temp_dx = (result_mat.cols - cy_Mat[i+1].cols)/2.0 - dxdy[i].first;
            temp_dy = (result_mat.rows - cy_Mat[i+1].rows)/2.0 - (dxdy[i].second - y_bias*(-dxdy[i].first));
            // temp_dy = -(dxdy[i].second - y_bias);
            result_mat = cylindrical_merge(cy_Mat[i+1],result_mat,temp_dx,temp_dy,0);
        }
    }
    imwrite("../panorama.jpg",result_mat);
    imshow("panorama",result_mat);
    waitKey(0);
    
}