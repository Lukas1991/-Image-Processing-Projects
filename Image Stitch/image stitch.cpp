#include <stdio.h>
#include <iostream>

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace cv;
using namespace std;


int main( int argc, char** argv )
{
// 载入图像
 Mat image1= imread("2.jpg");
 Mat image2= imread("1.jpg");
 Mat gray_image1;
 Mat gray_image2;
 //转换成灰度 
 cvtColor( image1, gray_image1, CV_RGB2GRAY );
 cvtColor( image2, gray_image2, CV_RGB2GRAY );

 imshow("first image",image1);
 imshow("second image",image2);

if( !gray_image1.data || !gray_image2.data )
 { std::cout<< " --(!) Error reading images " << std::endl; return -1; }

//使用SURF得到关键点
 int minHessian = 400;

SurfFeatureDetector detector( minHessian );

std::vector< KeyPoint > keypoints_object, keypoints_scene;

 detector.detect( gray_image1, keypoints_object );
 detector.detect( gray_image2, keypoints_scene );

//计算特征向量
 SurfDescriptorExtractor extractor;

Mat descriptors_object, descriptors_scene;

extractor.compute( gray_image1, keypoints_object, descriptors_object );
 extractor.compute( gray_image2, keypoints_scene, descriptors_scene );

//使用FLANN matcher配对特征描述子
 FlannBasedMatcher matcher;
 std::vector< DMatch > matches;
 matcher.match( descriptors_object, descriptors_scene, matches );

double max_dist = 0; double min_dist = 100;

//快速计算关键点之间最大最小距离
 for( int i = 0; i < descriptors_object.rows; i++ )
 { double dist = matches[i].distance;
 if( dist < min_dist ) min_dist = dist;
 if( dist > max_dist ) max_dist = dist;
 }

 printf("-- Max dist : %f \n", max_dist );
 printf("-- Min dist : %f \n", min_dist );

//使用好的匹配(距离小于3*min_dist)
 std::vector< DMatch > good_matches;

for( int i = 0; i < descriptors_object.rows; i++ )
 { if( matches[i].distance < 3*min_dist )
 { good_matches.push_back( matches[i]); }
 }
 std::vector< Point2f > obj;
 std::vector< Point2f > scene;

for( int i = 0; i < good_matches.size(); i++ )
 {
 //从好的配对点中得到关键点
 obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
 scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
 }

//发现Homography矩阵
 Mat H = findHomography( obj, scene, CV_RANSAC );
 //使用Homography矩阵合并图像
 cv::Mat result;
 warpPerspective(image1,result,H,cv::Size(image1.cols+image2.cols,image1.rows));
 cv::Mat half(result,cv::Rect(0,0,image2.cols,image2.rows));
 image2.copyTo(half);
 imshow( "Result", result );

 waitKey(0);
 return 0;
 }
