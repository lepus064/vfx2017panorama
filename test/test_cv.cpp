#include <opencv2/photo.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/core.hpp"
#include <opencv2/highgui.hpp>
#include "opencv2/features2d.hpp"
#include "opencv2/core/matx.hpp"
#include <dirent.h>

#include <algorithm>
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

Mat solve_d(const vector<double>& o, const vector<double>& v){
    auto A = Mat(3,3,CV_32F);
    for(auto i = 0; i < 3; i++){
        for(auto j = 0; j < 3; j++){
            A.at<float>(i,2-j) = pow(o[i],j);
        }
    }
    auto B = Mat(3,1,CV_32F);
    for(auto i = 0; i < 3; i++){
        B.at<float>(0,i) = v[i];
    }

    cout << A << endl;
    cout << B << endl;

    Mat x;
    solve(A,B,x,DECOMP_LU);
    cout << x << endl;
    double max_x = -x.at<float>(0,1)/(2*x.at<float>(0,0));
    double max_y = x.at<float>(0,0)*max_x*max_x + x.at<float>(0,1)*max_x +x.at<float>(0,2);
    cout << "Max x:" << max_x << endl;
    cout << "Max y:" << max_y << endl;
    
    return A;
}

int main(){
    vector<double> A{1,2,3};
    vector<double> B{2,5,3};
    
    auto a = solve_d(A,B);
}