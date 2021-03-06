#ifndef CYLINDRICAL_H
#define CYLINDRICAL_H

#include "opencv2/imgcodecs.hpp"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <cmath>

void cylindrical(cv::Mat& src, std::vector<cv::KeyPoint> &kps, double f);

cv::Mat cylindrical_merge(const cv::Mat& left, const cv::Mat& right, double x2x, double y2y, double angle);
std::pair<double,double> get_true_x_y(const cv::Mat& left, const cv::Mat& right, double x2x, double y2y);

#endif