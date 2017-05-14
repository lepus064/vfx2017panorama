#ifndef BRISK_DESCIPTOR_H
#define BRISK_DESCIPTOR_H

#include "opencv2/imgcodecs.hpp"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

struct pt_pair{
    cv::Point2f start;
    cv::Point2f end;
    double dist;
    double S_I;
    double E_I;
};

struct pt_value{
    cv::Point2f point;
    double I;
};


double get_gs_value(const cv::Mat& src, cv::Point pt);
cv::Mat brisk_short(const cv::Mat& src,cv::KeyPoint kp, double rad);
double brisk_compare(const cv::Mat &a, const cv::Mat &b);
int key_pair(const cv::Mat& a, const std::vector<cv::Mat> &b, int thres);

#endif