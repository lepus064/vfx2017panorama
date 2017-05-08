#ifndef BRISK_DISCIPTOR_H
#define BRISK_DISCIPTOR_H

#include "opencv2/imgcodecs.hpp"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <cmath>

double get_gs_value(const cv::Mat& src, cv::Point pt);
void brisk_short(const cv::Mat& src,cv::KeyPoint kp, double rad);
void brisk_compare();

#endif