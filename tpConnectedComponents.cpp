#include "tpConnectedComponents.h"
#include <cmath>
#include <algorithm>
#include <tuple>
#include <vector>
#include <stack> 

using namespace cv;
using namespace std;

// Careful : you have to use vector, and not Vector (with capital "V").
// If you make Vector<int> x = function() where function returns a vector, then the 1st element of x will be changed.


/**
    Performs a labeling of image connected component with 4 connectivity
    with a depth-first exploration.
    Any non zero pixel of the image is considered as present.
*/

Mat colorCC(Mat inputImage, Mat result, int px) {
    // finds the connected components of pixel color <px> in inputImage, and adds it to result.
    

    return result;

}

bool isInImage(Point2i p, Mat image) {
    //printf("Testing point (%d,%d)\n", p.x, p.y);
    return (p.x >=0 && p.x < image.rows && p.y >= 0 && p.y < image.cols);
}

bool hasSameColor(Point2i p1, Point2i p2, Mat image) {
    int p1Color = image.at<int>(p1.x, p1.y);
    int p2Color = image.at<int>(p2.x, p2.y);
    return (p1Color == p2Color);
}


// If you want to use a *set* of Points, you need to supply a compare operation for points.
// Also you have to use an iterator, syntax is complicated and heavy
// So I choose to use a vector.

vector<Point2i> getNeighbours(Mat image, Point2i p) {
    vector<Point2i> result ={};
    vector<Point2i> neighbours = {{-1,0}, {0,-1}, {0,1}, {1,0}}; 
    for(Point2i neighbour: neighbours) {
        neighbour += p;
        if (isInImage(neighbour, image))
             result.push_back(neighbour);    
    }
    return result;
}

vector<Point2i>getUnvisitedNeighboursOfSameColour(Mat image, vector<vector<bool>> &visited, Point2i p) {   
    vector<Point2i> result ={};
    vector<Point2i> neighbours = getNeighbours(image, p);
    for(Point2i neighbour: neighbours) {
        if (! visited[neighbour.x][neighbour.y] && hasSameColor(p, neighbour, image)) {
            result.push_back(neighbour);
        }
    }
    return result;
}


bool contains(vector<Point2i> v, Point2i element) { // returns true if element is in vector, false otherwise
    return std::find(v.begin(), v.end(), element) != v.end();
}

bool contains(vector<int> v, int element) {
    return std::find(v.begin(), v.end(), element) != v.end();
}


vector<Point2i> uniqFusion(vector<Point2i> v1, vector<Point2i> v2) {// Fusion with unicity
    for (Point2i p2: v2) {
        if (!contains (v1, p2)) {
            v1.push_back(p2);
        }
    } 
    return v1;
}

vector<int> uniqFusion(vector<int> v1, vector<int> v2) {// Fusion with unicity
    for (int p2: v2) {
        if (!contains (v1, p2)) {
            v1.push_back(p2);
        }
    } 
    return v1;
}


int extractCC(Mat image, Mat res, vector<vector<bool>> &visited, Point2i p, int compteur) {
    int cc_size = 0;
    vector<Point2i> cc_neighbours = {p};
    while (cc_neighbours.size() > 0) {
        cc_size ++;
        // Pop first element
        Point2i v = cc_neighbours[0];
        cc_neighbours.erase(cc_neighbours.begin());

        // 1. We put its res location to <compteur>
        res.at<int>(v.x, v.y) = compteur;

        // 2. we mark it as visited
        visited[v.x][v.y] = true;

        // 3 If it has neighbours that are not in the rest of the array, we add them to it.
        vector<Point2i> new_neighbours = getUnvisitedNeighboursOfSameColour(image, visited, v);
        cc_neighbours = uniqFusion(cc_neighbours, new_neighbours);
    } 
    return cc_size;
}


Mat ccLabel(Mat image)
{
    Mat res = Mat::zeros(image.rows, image.cols, CV_32SC1); 
    vector<vector<bool>> visited(image.rows, vector<bool>(image.cols)); // initialized to 0 = false
    int compteur = 0;
    printf("Image size : %dx%d\n", image.rows, image.cols);
    
    for (int i = 0 ; i < image.rows; i++) {
        for(int j = 0; j < image.cols; j++) {
            if ( ! visited[i][j]) {
                visited[i][j] = true;
                if (image.at<int>(i,j) != 0) { 
                    compteur += 1;
                    int cc_size = extractCC(image, res, visited, {i,j}, compteur);
                    printf("Connected Component %d extracted from pixel (%d,%d).  Size : %d\n", compteur, i, j, cc_size);
                }
            }
        }
    }
    printf("Algorithm ended. Compteur is %d \n", compteur);
    return res;
}




/**
    Deletes the connected components (4 connectivity) containg less than size pixels.
*/



cv::Mat ccAreaFilter(cv::Mat image, int size)
{
    Mat res = image.clone();
    Mat cc = Mat::zeros(image.rows, image.cols, CV_32SC1); 
    vector<vector<bool>> visited(image.rows, vector<bool>(image.cols)); // initialized to 0 = false
    int compteur = 0;
    vector<int>cc_sizes = {};
    
    // Extract the components for all points
    for (int i = 0 ; i < image.rows; i++) {
        for(int j = 0; j < image.cols; j++) {
            if ( ! visited[i][j]) {
                visited[i][j] = true;
                if (image.at<int>(i,j) != 0) { 
                    compteur += 1;
                    int cc_size = extractCC(image, cc, visited, {i,j}, compteur);
                    cc_sizes.push_back(cc_size);
                     //printf("Connected Component %d extracted from pixel (%d,%d).  Size : %d\n", compteur, i, j, cc_size);
                }
            }
        }
    }

    // delete components that are less than <size>
    for (int i = 0 ; i < (int) cc_sizes.size(); i++) {
        if (cc_sizes[i] < size) {
            // eliminate the CC. 
            // If its location is i in cc_size, then it has been marked as (i+1) in cc.
            // Example : cc_size[0] contains the size of CC number 1.
            res.setTo(0, cc == (i+1));
        }
    }

    return res;
}

bool firstPass(Mat image, Mat res, Point2i p, int compteur, vector<vector<int>> &equivalenceArray) { // Returns true if compteur needs to be increased
    //printf("f");
    if (image.at<int>(p.x, p.y) == 0) return false;
    // So p is not background.

    if (p.x == 0 && p.y == 0) { // Neither above nor left exist:
        res.at<int>(p.x, p.y) = compteur;
        return true;
    }
    if (p.x == 0) { // No left neighbour, but above exists
        int aboveLabel = res.at<int>(p.x, p.y - 1);

        if (aboveLabel == 0 ) { // above not labeled
            res.at<int>(p.x, p.y) = compteur;
            return true; 
        }
        else { // above labeled
            res.at<int>(p.x, p.y) =  aboveLabel;
            return false;
        }

    }
    if (p.y == 0) { // No above neighbour, but left exists
        int leftLabel = res.at<int>(p.x - 1, p.y);

        if (leftLabel == 0) { // left not labeled
            res.at<int>(p.x, p.y) = compteur;
            return true;
        } 
        else { // left labeled
            res.at<int>(p.x, p.y) = leftLabel;
            return false;
        }
    }

    // Both above and left exist
    int aboveLabel = res.at<int>(p.x, p.y - 1);
    int leftLabel = res.at<int>(p.x - 1, p.y);

    if (aboveLabel == 0 && leftLabel == 0) { // None assigned
         res.at<int>(p.x, p.y) = compteur;
         return true;
    }
   
    if (aboveLabel == 0 && leftLabel != 0) { // only left exists
         res.at<int>(p.x, p.y) = leftLabel;
         return false;
    }

    if (aboveLabel != 0 && leftLabel == 0) { // only right exists
         res.at<int>(p.x, p.y) = aboveLabel;
         return false;
    }
    // Both values exist.
    // If they are the same :
    if (aboveLabel == leftLabel) {
        res.at<int>(p.x, p.y) = aboveLabel;
        return false;
    }
    // else 
    int smallestLabel = min(aboveLabel, leftLabel);
    int largestLabel = max(aboveLabel, leftLabel);
    res.at<int>(p.x, p.y) = smallestLabel;

    // We must now indicate the equiv smallestLabel = largestLabel
    vector<int> joinIndex = {};
    for (int k = 0 ; k < (int) equivalenceArray.size(); k++ ) {
        
         if (contains(equivalenceArray[k], smallestLabel) &&  contains(equivalenceArray[k], largestLabel)) { // both are already registered !
            //printf("This CC is already known\n");
            return false;
         }

        if (contains(equivalenceArray[k], smallestLabel) && ! contains(equivalenceArray[k], largestLabel)) { // smallestLabel is in the array, but largestLabel isn't
            equivalenceArray[k].push_back(largestLabel);
            joinIndex.push_back(k);
            //printf("%d added to equiv[%d] \n", largestLabel, k);
        }
        if (contains(equivalenceArray[k], largestLabel) && ! contains(equivalenceArray[k], smallestLabel)) { // largestLabel is in the array, but smallestLabel isn't
            equivalenceArray[k].push_back(smallestLabel);
            joinIndex.push_back(k);
            //printf("%d added to equiv[%d] \n", largestLabel, k);
        }

    }
    //printf("joinIndex's size is now %d\n", (int) joinIndex.size());

    if (joinIndex.size() > 1) { 
        equivalenceArray[joinIndex[0]] = uniqFusion(equivalenceArray[joinIndex[0]], equivalenceArray[joinIndex[1]]);
        equivalenceArray.erase(equivalenceArray.begin() + joinIndex[1]); 
        //printf("Deleted one class\n");
    }
    if (joinIndex.size() == 0) {
        vector<int> newVec;
        newVec.push_back(smallestLabel);
        newVec.push_back(largestLabel);
        equivalenceArray.push_back(newVec);
        //printf("New class {%d, %d} added\n", smallestLabel, largestLabel);
    }

    return false;
}

int minElement(vector<int> &v) {
    if (v.size() == 0) return 0;
    int res = v[0];
    for (int element: v) {
        if (element < res)
            res = element;
    }
    return res;
}

void secondPass(Mat res, Point2i p, vector<vector<int>> &equivalenceArray) {
    if (res.at<int>(p.x, p.y) == 0) return;
    for (int k = 0; k < (int) equivalenceArray.size(); k++) {
        if (contains(equivalenceArray[k], res.at<int>(p.x, p.y))) {
            
        //res.at<int>(p.x, p.y) = equivalenceArray[k][0]; // equivalenceArray has been sorted
            res.at<int>(p.x, p.y) = k + 1;
            return;
        }

    }
}

/**
    Performs a labeling of image connected component with 4 connectivity using a
    2 pass algorithm.
    Any non zero pixel of the image is considered as present.
*/
cv::Mat ccTwoPassLabel(cv::Mat image)
{
    Mat res = Mat::zeros(image.rows, image.cols, CV_32SC1); // 32 int image

    int compteur = 1;
    vector<vector<int>> equivalenceArray = {}; // will contain the equivalences : equivalenceArray[0] = {7,14} means component 7 and component 14 are the same.
    
    // First pass on all pixels

    for(int i = 0; i < image.rows; i++) {     
        for (int j = 0 ; j < image.cols; j++) {
            if (firstPass(image, res, Point2i(i,j), compteur, equivalenceArray) ) {
                compteur++;
            }
        }
    }
    
    // Trick : We now add the "equivalence classes" of 1 element.
    // This will allow our function "secondPass" to easily find the correct number to give to each class.

    for(int u = 1; u < compteur; u++) {
        bool shouldWeAdd = true;
        for (vector<int> equiv: equivalenceArray) {
            if (contains(equiv, u)) {
                shouldWeAdd = false;
            }
        }
        if (shouldWeAdd)
            equivalenceArray.push_back({u});
    }
    // End of the trick.
    printf("Total number of connected components : %d\n", (int) equivalenceArray.size());
    
    // Now we sort the equivalenceArrays in increasing order
    for (int i = 0 ; i < (int) equivalenceArray.size(); i++) {
        sort(equivalenceArray[i].begin(), equivalenceArray[i].end());
    }

    std::sort(equivalenceArray.begin(), equivalenceArray.end(),
          [](const std::vector<int>& a, const std::vector<int>& b) {
     return a[0] < b[0];
    });
    
    // If you need to print the equivalenceArrays (debug ):

    /*
     for (int i = 0; i < (int) equivalenceArray.size(); i++) {     
        for (int j = 0 ; j < (int) equivalenceArray[i].size(); j++) {
            printf("%d,", equivalenceArray[i][j]);
        }
        printf("\n");
     }
     */

    // We now apply the secondPass
    for (int i = 0 ; i < image.rows; i++) {
        for(int j = 0; j < image.cols; j++) {
            secondPass(res, Point2i(i,j), equivalenceArray);   
        }
    }

    return res;
}