
#include "tpConvolution.h"
#include <cmath>
#include <algorithm>
#include <tuple>
using namespace cv;
using namespace std;


float pixelMeanValue(cv::Mat image, int k, int i, int j) { /*k size of window, pixel of coord i, j*/
   int min_row = max(i - k, 0);
   int max_row = min(i + k, image.rows - 1);
   int min_col = max(j - k, 0);
   int max_col = min(j + k, image.cols - 1);

    float sum_px = 0.0;

    for (int m = min_row ; m <= max_row; m++ ) {
        for (int n = min_col; n <= max_col; n++) {
            //printf("adding px (%d,%d) of value %f\n", m, n, image.at<float>(m,n));
            sum_px += image.at<float>(m,n);
        }
        
    }

    // WHY??????????
    int size_window = (2*k+1) * (2*k+1);
    //int size_window = (max_row - min_row + 1) * (max_col - min_col + 1);
    //printf("Size of window is %d and sum %f\n", size_window, sum_px);
    float avg = sum_px / (float)size_window;
    //printf("avg[%d,%d] %f\n", i, j, avg);
    return avg;
}

/**
    Compute a mean filter of size 2k+1.

    Pixel values outside of the image domain are supposed to have a zero value.
*/
cv::Mat meanFilter(cv::Mat image, int k){
    printf("Image size : %dx%d\n", image.rows, image.cols);

    Mat res = image.clone();

     for (int i = 0 ; i < image.rows; i++) {
        for(int j = 0; j < image.cols; j++) {
            res.at<float>(i,j) = pixelMeanValue(image, k, i, j);
        }
     }
    //float pxMean = pixelMeanValue(image, k, 0, 0);
    //printf("mean is %f\n", pxMean);
    return res;
}


float pxValue(Mat mat, int i, int j) { 
    if (i < 0 || j < 0 || i >= mat.rows || j >= mat.cols)
        return 0;
    else
        return mat.at<float>(i,j);
}

float pxConvolution(Mat image, Mat kernel, int i, int j) {
    
    int k = (kernel.rows - 1) / 2;
    float sum_px = 0.0;

    for (int m = -k; m <=k; m++) {
        for (int n = -k; n <=k; n++) {
            sum_px += pxValue(image, i + m, j + n) * kernel.at<float>(m + k, n + k);
        }
    }
    return sum_px;
    //return sum_px / (float) (kernel.rows * kernel.rows);


}



/**
    Compute the convolution of a float image by kernel.
    Result has the same size as image.
    
    Pixel values outside of the image domain are supposed to have a zero value.
*/
Mat convolution(Mat image, cv::Mat kernel)
{
    Mat res = image.clone();
    printf("Image size : %dx%d\n", image.rows, image.cols);

     for (int i = 0 ; i < image.rows; i++) {
        for(int j = 0; j < image.cols; j++) {
            res.at<float>(i,j) = pxConvolution(image, kernel, i, j);
        }
     }
    //float pxMean = pxConvolution(image, kernel, 30, 30);
    //printf("mean is %f\n", pxMean);
    return res;
}

/**
    Compute the sum of absolute partial derivative according to Sobel's method
*/
cv::Mat edgeSobel(cv::Mat image)
{
    Mat res = image.clone();
    Mat sobel_x = (Mat_<float>(3,3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);
    Mat sobel_y = (Mat_<float>(3,3) << 1, 2, 1, 0, 0, 0, -1, -2, -1);
     for (int i = 0 ; i < image.rows; i++) {
        for(int j = 0; j < image.cols; j++) {
            float dfdx = pxConvolution(image, sobel_x, i, j);
            float dfdy = pxConvolution(image, sobel_y, i, j);
            res.at<float>(i,j) = abs(dfdx) + abs(dfdy);
            //res.at<float>(i,j) = sqrt(pow(dfdx, 2) + pow(dfdy, 2));
        }
     }

    float dfdx1 = pxConvolution(image, sobel_x, 10, 10);
    float dfdy1 = pxConvolution(image, sobel_y, 10, 10);
    printf("values : %f and %f\n", dfdx1, dfdy1);

    
    for (int i = 0 ; i < sobel_y.rows; i++) {
        for(int j = 0; j < sobel_y.cols; j++) {
            printf("%d\n", sobel_y.at<int>(i,j));
        }
     }
     

    return res;
}

/**
    Value of a centered gaussian of variance (scale) sigma at point x.
*/
float gaussian(float x, float sigma2)
{
    return 1.0/(2*M_PI*sigma2)*exp(-x*x/(2*sigma2));
}

/**
    Performs a bilateral filter with the given spatial smoothing kernel 
    and a intensity smoothing of scale sigma_r.

*/



float pxBilateralFilter(Mat image, Mat kernel, int i, int j, double sigma_r) {
    
    int k = (kernel.rows - 1) / 2;
    float sum_px = 0.0;
    float norm_fact = 0.0;
    float pxVal = image.at<float>(i,j);

    for (int m = -k; m <=k; m++) {
        for (int n = -k; n <=k; n++) {
            if (((i+m) >= 0) && ((j+n) >=0) && ((i+m) < image.rows) && ((j+n) < image.cols)) {
                float neighVal = image.at<float>(i + m, j  + n);
    
                float temp = kernel.at<float>(m + k, n + k);
                sum_px += temp * gaussian(abs(pxVal - neighVal), (float) pow(sigma_r,2)) * neighVal;
                norm_fact += temp * gaussian(abs(pxVal - neighVal), (float) pow(sigma_r,2));
            }
        }
    }
    return sum_px / norm_fact;
    //return sum_px / (float) (kernel.rows * kernel.rows);


}

cv::Mat bilateralFilter(cv::Mat image, cv::Mat kernel, double sigma_r)
{
    Mat res = image.clone();

   for (int i = 0; i < res.rows; i++) {
       for (int j = 0; j < res.cols; j ++) {
           res.at<float>(i,j) = pxBilateralFilter(image, kernel, i, j, sigma_r);
       }
   }

    return res;
}
