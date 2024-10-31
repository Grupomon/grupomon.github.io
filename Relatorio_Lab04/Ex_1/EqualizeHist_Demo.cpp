/**
 * @function EqualizeHist_Demo.cpp
 * @brief Demo code for equalizeHist function
 * @author OpenCV team
 */

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace cv;
using namespace std;

/**
 * @function main
 */
int main( int argc, char** argv )
{
    //! [Load image]
    CommandLineParser parser( argc, argv, "{@input | lena.jpg | input image}" );
    Mat src = imread( samples::findFile( parser.get<String>( "@input" ) ), IMREAD_COLOR );
    if( src.empty() )
    {
        cout << "Could not open or find the image!\n" << endl;
        cout << "Usage: " << argv[0] << " <Input image>" << endl;
        return -1;
    }
    //! [Load image]

    //! [Convert to grayscale]
    Mat gray;
    cvtColor( src, gray, COLOR_BGR2GRAY );
    //! [Convert to grayscale]

    //! [Apply Histogram Equalization]
    Mat dst;
    equalizeHist( gray, dst );
    //! [Apply Histogram Equalization]

    //! [Display results]
    imshow( "Source Image", src );
    imshow( "Gray Image", gray );
    imshow( "Equalized Image", dst );

    // Save the gray and equalized images
    imwrite("input_gray_image.jpg", gray);
    imwrite("equalized_image.jpg", dst);
    cout << "Input gray image saved as 'input_gray_image.jpg'." << endl;
    cout << "Equalized image saved as 'equalized_image.jpg'." << endl;
    //! [Display results]

    //! [Wait until user exits the program]
    waitKey();
    //! [Wait until user exits the program]

    return 0;
}

