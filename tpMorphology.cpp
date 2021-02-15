#include "tpMorphology.h"
#include <cmath>
#include <algorithm>
#include <tuple>
#include <limits>
#include "common.h"
using namespace cv;
using namespace std;


/**
 * k size of window, pixel of coord i, j
 * */
float pixelMedianValue(cv::Mat image, int k, int n, int m) {

    std::vector<float> pixelsValues;

    int min_row = max(n - k, 0);
    int max_row = min(n + k, image.rows - 1);
    int min_col = max(m - k, 0);
    int max_col = min(m + k, image.cols - 1);

    for (int i = min_row ; i <= max_row; i++) 
    {
        for (int j = min_col; j <= max_col; j++)    
        {
            pixelsValues.push_back(image.at<float>(i,j));
        }
        
    }

    std::sort(pixelsValues.begin(), pixelsValues.end());

    float median = 0;
    if(pixelsValues.size() % 2 == 0)
    {
        median = (pixelsValues[pixelsValues.size()/2-1] + pixelsValues[pixelsValues.size()/2])/2;
    }
    else 
    {
        median = pixelsValues[pixelsValues.size()/2];
    }

    return median;
}


/**
 * k size of window, pixel of coord i, j
 * */
float pixelDilate(cv::Mat image, cv::Mat structuringElement,int i ,int j) {

    std::vector<float> pixelsValues = {};

    int window_width = (structuringElement.rows - 1) / 2;
    int window_height = (structuringElement.cols - 1) / 2;

    for (int x = -window_width; x <= window_width; x++) 
    {
        for (int y = -window_height; y <= window_height; y++)    
        {
            if((i+x >= 0 && i+x < image.rows && j+y >= 0 && j+y < image.cols) && structuringElement.at<float>(x + window_width, y + window_height) == 1)
            {
                pixelsValues.push_back(image.at<float>(i+x, j+y));
            }
        }
    }

    float max = *max_element(pixelsValues.begin(), pixelsValues.end());

    return max;
}

/**
    Compute a median filter of the input float image.
    The filter window is a square of (2*size+1)*(2*size+1) pixels.

    Values outside image domain are ignored.

    The median of a list l of n>2 elements is defined as:
     - l[n/2] if n is odd 
     - (l[n/2-1]+l[n/2])/2 is n is even 
*/
Mat median(Mat image, int size)
{
    Mat res = image.clone();
    assert(size>0);
    /********************************************
                YOUR CODE HERE
    *********************************************/
    for (int i = 0 ; i < image.rows; i++) 
    {
        for(int j = 0; j < image.cols; j++) 
        {
            res.at<float>(i,j) = pixelMedianValue(image, size, i, j);
        }
     }
    /********************************************
                END OF YOUR CODE
    *********************************************/
    return res;
}


/**
    Compute the erosion of the input float image by the given structuring element.
    Pixel outside the image are supposed to have value 1.
*/
Mat erode(Mat image, Mat structuringElement)
{
    Mat res = image.clone();
    /********************************************
                YOUR CODE HERE
    *********************************************/
   for (int i = 0 ; i < image.rows; i++) 
    {
        for(int j = 0; j < image.cols; j++) 
        {
            res.at<float>(i,j) = -pixelDilate(-image, structuringElement, i, j);
        }
    }
    
    /********************************************
                END OF YOUR CODE
    *********************************************/
    return res;
}


/**
    Compute the dilation of the input float image by the given structuring element.
     Pixel outside the image are supposed to have value 0
*/
Mat dilate(Mat image, Mat structuringElement)
{
    //Mat res = Mat::zeros(1,1,CV_32FC1);
    Mat res = image.clone();
    /********************************************
                YOUR CODE HERE
        hint : 1 line of code is enough
    *********************************************/
    for (int i = 0 ; i < image.rows; i++) 
    {
        for(int j = 0; j < image.cols; j++) 
        {
            res.at<float>(i,j) = pixelDilate(image, structuringElement, i, j);
        }
    }
    /********************************************
                END OF YOUR CODE
    *********************************************/
    return res;
}


/**
    Compute the opening of the input float image by the given structuring element.
*/
Mat open(Mat image, Mat structuringElement)
{

    //Mat res = Mat::zeros(1,1,CV_32FC1);
    /********************************************
                YOUR CODE HERE
        hint : 1 line of code is enough
    *********************************************/
    return dilate(erode(image, structuringElement), structuringElement);
    
    /********************************************
                END OF YOUR CODE
    *********************************************/
    //return res;
}


/**
    Compute the closing of the input float image by the given structuring element.
*/
Mat close(Mat image, Mat structuringElement)
{

    //Mat res = Mat::zeros(1,1,CV_32FC1);
    /********************************************
                YOUR CODE HERE
        hint : 1 line of code is enough
    *********************************************/
    return erode(dilate(image, structuringElement), structuringElement);
    /********************************************
                END OF YOUR CODE
    *********************************************/
    //return res;
}


/**
    Compute the morphological gradient of the input float image by the given structuring element.
*/
Mat morphologicalGradient(Mat image, Mat structuringElement)
{

    //Mat res = Mat::zeros(1,1,CV_32FC1);
    /********************************************
                YOUR CODE HERE
        hint : 1 line of code is enough
    *********************************************/
    return (image - erode(image, structuringElement)) - dilate(image, structuringElement) - image;
    /********************************************
                END OF YOUR CODE
    *********************************************/
    //return res;
}

