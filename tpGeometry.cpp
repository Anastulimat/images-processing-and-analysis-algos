#include "tpGeometry.h"
#include <cmath>
#include <algorithm>
#include <tuple>
using namespace cv;
using namespace std;

/**
    Transpose the input image,
    ie. performs a planar symmetry according to the
    first diagonal (upper left to lower right corner).
*/
Mat transpose(Mat image)
{
    Mat res = Mat::zeros(image.cols, image.rows, CV_32FC1);

     for (int i = 0 ; i < image.rows; i++) {
        for(int j = 0; j < image.cols; j++) {
            res.at<int>(j,i) = image.at<int>(i,j); 
        }
     }

    // printf("%f\n",interpolate_nearest(image, 0.75, 1.245));
    // printf("%f\n", image.at<int>(1,2));
    return res;
}

/**
    Compute the value of a nearest neighbour interpolation
    in image Mat at position (x,y)
*/
float interpolate_nearest(Mat image, float x, float y)
{
    float v = 0;
    int a = (int) round(x);
    int b = (int) round(y);
    v = image.at<float>(a,b);
    return v;

}


/**
    Compute the value of a bilinear interpolation in image Mat at position (x,y)
*/
float interpolate_bilinear(Mat image, float x, float y)
{
    int x1 = int(x);
    int y1 = int(y);

    int x2 = x1 + 1;
    int y2 = y1 + 1;

    float alpha = image.at<float>(x2, y1) - image.at<float>(x1, y1);
    float beta = image.at<float>(x2, y2) - image.at<float>(x1, y2);


    float f_x_y1 = image.at<float>(x1, y1) + alpha * (x - x1);
    float f_x_y2 = image.at<float>(x1, y2) + beta * (x - x1);

    float gamma = f_x_y2 - f_x_y1; 
    

    float v = f_x_y1 + (y - y1) * gamma;
  
    return v;
}

/**
    Multiply the image resolution by a given factor using the given interpolation method.
    If the input size is (h,w) the output size shall be ((h-1)*factor, (w-1)*factor)
*/
Mat expand(Mat image, int factor, float(* interpolationFunction)(cv::Mat image, float y, float x))
{
    assert(factor>0);
    Mat res = Mat::zeros((image.rows -1) * factor,(image.cols -1) * factor,CV_32FC1);

    for (int i = 0 ; i < res.rows; i++) {
        for(int j = 0; j < res.cols; j++) {
            res.at<float>(i,j) = interpolationFunction(image, (float) i  / factor, (float) j / factor);
        }
    }


    return res;
}

/**
    Performs a rotation of the input image with the given angle (clockwise) and the given interpolation method.
    The center of rotation is the center of the image.

    Ouput size depends of the input image size and the rotation angle.

    Output pixels that map outside the input image are set to 0.
*/

Mat rotate(Mat image, float angle, float(* interpolationFunction)(cv::Mat image, float y, float x))
{
    int height = image.rows;
    int width = image.cols;
    
    // transforming angle to alpha € [0, Pi]
    float alpha = angle * M_PI / 180; 
    printf("alpha is %f\n", alpha);
    double twoPi = 2.0 * M_PI;
    printf("floor is %f\n", floor(alpha / twoPi));
    alpha = alpha - twoPi * floor(alpha / twoPi);


    float old_center_x = (float) (width - 1) / 2.0;
    float old_center_y = (float) (height - 1) / 2.0;

    int newWidth, newHeight;

    if (alpha < M_PI / 2.0) {
        newWidth = int((width-1) * cos(alpha) + (height-1) * sin(alpha)) + 1 ;
        newHeight = int((width-1) * sin(alpha) + (height-1) * cos(alpha)) + 1;
    } else {
        newWidth = int( - (width - 1) * cos(alpha) + (height - 1) * sin(alpha) );
        newHeight = int( (width - 1) * sin(alpha) - (height - 1) * cos(alpha) );
    }

    Mat res = Mat::zeros(newHeight, newWidth,CV_32FC1);

    //float center_x = ((width-1) * cos(alpha) + (height-1) * sin(alpha)) / 2.0;
    //float center_y =  ((width-1) * sin(alpha) + (height-1) * cos(alpha)) / 2.0;

    float center_x = (float) (newWidth - 1) / 2.0;
    float center_y = (float) (newHeight - 1) / 2.0;


    for (int i = 0 ; i < res.rows; i++) {
        for(int j = 0; j < res.cols; j++) {

            float u = ((float) j) - center_x;
            float v = ((float) i) - center_y;

            float x = u * cos(alpha) - v * sin(alpha);
            float y = u * sin(alpha) + v * cos(alpha);

            x = x + old_center_x;
            y = y + old_center_y;

            if (x > 0 && x < image.cols - 1  && y > 0 && y < image.rows - 1 ) {
                res.at<float>(i, j) = interpolationFunction(image, y, x); 
            }
        }
    }

    return res;

}