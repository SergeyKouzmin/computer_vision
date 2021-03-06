#include "opencv2/video/tracking_c.h"
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/highgui/highgui_c.h"
#include "opencv2/imgproc.hpp"
#include <time.h>
#include <stdio.h>
#include <ctype.h>
//C++
#include <iostream>
#include <sstream>
#include <vector>

using namespace cv;
using namespace std;

const float PERIM_SCALE = 10.0;
const int CLOSE_ITR = 1;
const int MAX_OBJECT_TRACK_POINT_COUNT = 400;

typedef vector<CvPoint> ObjectTrack;

typedef std::vector<cv::Point> Contour;

typedef std::vector< std::vector<cv::Point> > Contours;

// Global variables
Contours maskContours;

ObjectTrack points;

// various tracking parameters (in seconds)
const double MHI_DURATION = 1;
const double MAX_TIME_DELTA = 0.5;
const double MIN_TIME_DELTA = 0.05;
// number of cyclic frame buffer used for motion detection
// (should, probably, depend on FPS)
const int N = 4;

// ring image buffer
IplImage **buf = 0;
int last = 0;

// temporary images
IplImage *mhi = 0; // MHI
IplImage *orient = 0; // orientation
IplImage *mask = 0; // valid orientation mask
IplImage *segmask = 0; // motion segmentation map
CvMemStorage* storage = 0; // temporary storage


void throwError(const std::string& msg) {
    cerr << "Error: " << msg << endl;
    exit(EXIT_FAILURE);
}

cv::Mat morphologyFilter(cv::Mat& img) {
        if (!img.isContinuous()) {
                throwError("Parammeter 'img' in 'morphologyFilter' must be continuous");
        }

        cv::Mat buf(img.size(), img.type());
        buf.setTo(0);
        cv::morphologyEx(img, buf, MORPH_OPEN, cv::Mat(), cv::Point(-1, -1), CLOSE_ITR);
        cv::morphologyEx(img, buf, MORPH_CLOSE, cv::Mat(), cv::Point(-1, -1), CLOSE_ITR);

        return buf;
}

cv::Mat connectedComponentsFilter(cv::Mat& curFrame, cv::Mat& img) {

    if (!img.isContinuous()) {
        throwError("Parammeter 'img' in 'connectedComponentsFilter' must be continuous");
    }

    //morphology_(img);

    maskContours.clear();

    // Отрисовать найденные области обратно в маску
    cv::Mat result(img.size(), img.type());
    result.setTo(0);

    cv::findContours(img, maskContours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
    size_t i = 0;

    while (i < maskContours.size()) {
        Contour& contour = maskContours[i];

        cv::Mat contourMat(contour, false);
        double len = cv::arcLength(contourMat, true);

        if (len * PERIM_SCALE < img.size().height + img.size().width) {
            // Отбрасываем контуры со слишком маленьким периметром.
            maskContours.erase(maskContours.begin() + i);
        } else {
            // Достаточно большие контуры аппроксимируем указанным методом.
            Contour newContour;

            // Методы аппроксимации.
            //cv::approxPolyDP(contourMat, newContour, CHAIN_APPROX_SIMPLE, true);
            cv::convexHull(contourMat, newContour, true);

            cv::Mat newContourMat(newContour, false);
            Rect boundingRect = cv::boundingRect(newContourMat);
            cv::rectangle(curFrame, boundingRect, cv::Scalar(255));

            maskContours[i] = newContour;

            i++;

            //points.push_back(CvPoint(boundingRect.x + boundingRect.width / 2, boundingRect.y + boundingRect.height / 2));
        }
    }


    if (!maskContours.empty()) { // Обходим баг OpenCV 2.1.0; в 2.3.1 он уже исправлен.
        cv::drawContours(result, maskContours, -1, cv::Scalar(255), FILLED);

    }
    return result;
}


void drawObjects(Mat& curFrame, Mat& img) {

    Mat result;// = img.clone();
    //blur( img, result, Size(3,3) );
    //Canny( result, result, 20, 300, 5);
    Mat resultMorph = morphologyFilter(img);
    cv::medianBlur(resultMorph, resultMorph, 5);
    Mat resultConnectComp = connectedComponentsFilter(curFrame, resultMorph);

    /// Show in a window
    imshow( "Contours", resultConnectComp);
}


void drawPoints(IplImage* img, ObjectTrack objectTrack) {
    for(int i = 0; i < objectTrack.size(); ++i) {
        CvPoint point = objectTrack[i];
        cvCircle( img, point, 1, cvScalar(0, 255, 0, 0), 3, CV_AA, 0 );
    }

}

// parameters:
//  img - input video frame
//  dst - resultant motion picture
//  args - optional parameters
static void  update_mhi( IplImage* img, IplImage* dst, int diff_threshold )
{

    double timestamp = (double)clock()/CLOCKS_PER_SEC; // get current time in seconds
    CvSize size = cvSize(img->width,img->height); // get current frame size
    int i, idx1 = last, idx2;
    IplImage* silh;
    CvSeq* seq;
    CvRect comp_rect;
    double count;
    double angle;
    CvPoint center;
    double magnitude;
    CvScalar color;

    // allocate images at the beginning or
    // reallocate them if the frame size is changed
    if( !mhi || mhi->width != size.width || mhi->height != size.height ) {
        if( buf == 0 ) {
            buf = (IplImage**)malloc(N*sizeof(buf[0]));
            memset( buf, 0, N*sizeof(buf[0]));
        }

        for( i = 0; i < N; i++ ) {
            cvReleaseImage( &buf[i] );
            buf[i] = cvCreateImage( size, IPL_DEPTH_8U, 1 );
            cvZero( buf[i] );
        }
        cvReleaseImage( &mhi );
        cvReleaseImage( &orient );
        cvReleaseImage( &segmask );
        cvReleaseImage( &mask );

        mhi = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        cvZero( mhi ); // clear MHI at the beginning
        orient = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        segmask = cvCreateImage( size, IPL_DEPTH_32F, 1 );
        mask = cvCreateImage( size, IPL_DEPTH_8U, 1 );
    }

    cvCvtColor( img, buf[last], CV_BGR2GRAY ); // convert frame to grayscale

    idx2 = (last + 1) % N; // index of (last - (N-1))th frame
    last = idx2;

    silh = buf[idx2];
    cvAbsDiff( buf[idx1], buf[idx2], silh ); // get difference between frames

    cvThreshold( silh, silh, diff_threshold, 1, CV_THRESH_BINARY ); // and threshold it
    cvUpdateMotionHistory( silh, mhi, timestamp, MHI_DURATION ); // update MHI

    // convert MHI to blue 8u image
    cvCvtScale( mhi, mask, 255./MHI_DURATION,
                (MHI_DURATION - timestamp)*255./MHI_DURATION );
    cvZero( dst );
    cvMerge( mask, 0, 0, 0, dst );

    // calculate motion gradient orientation and valid orientation mask
    cvCalcMotionGradient( mhi, mask, orient, MAX_TIME_DELTA, MIN_TIME_DELTA, 3 );

    if( !storage )
        storage = cvCreateMemStorage(0);
    else
        cvClearMemStorage(storage);

    // segment motion: get sequence of motion components
    // segmask is marked motion components map. It is not used further
    seq = cvSegmentMotion( mhi, segmask, storage, timestamp, MAX_TIME_DELTA );

    // iterate through the motion components,
    // One more iteration (i == -1) corresponds to the whole image (global motion)
    for( i = -1; i < seq->total; i++ ) {

        if( i < 0 ) { // case of the whole image
            comp_rect = cvRect( 0, 0, size.width, size.height );
            color = CV_RGB(255,255,255);
            magnitude = 100;
        }
        else { // i-th motion component
            comp_rect = ((CvConnectedComp*)cvGetSeqElem( seq, i ))->rect;
            if( comp_rect.width + comp_rect.height < 100 ) // reject very small components
                continue;
            color = CV_RGB(255,0,0);
            magnitude = 30;
        }

        // select component ROI
        cvSetImageROI( silh, comp_rect );
        cvSetImageROI( mhi, comp_rect );
        cvSetImageROI( orient, comp_rect );
        cvSetImageROI( mask, comp_rect );

        // calculate orientation
        angle = cvCalcGlobalOrientation( orient, mask, mhi, timestamp, MHI_DURATION);
        angle = 360.0 - angle;  // adjust for images with top-left origin

        count = cvNorm( silh, 0, CV_L1, 0 ); // calculate number of points within silhouette ROI

        cvResetImageROI( mhi );
        cvResetImageROI( orient );
        cvResetImageROI( mask );
        cvResetImageROI( silh );

        // check for the case of little motion
        if( count < comp_rect.width*comp_rect.height * 0.05 )
            continue;

        // draw a clock with arrow indicating the direction
        center = cvPoint( (comp_rect.x + comp_rect.width/2),
                          (comp_rect.y + comp_rect.height/2) );
        points.push_back(center);


        /*cvCircle( dst, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
        cvLine( dst, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
                cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );
        */
        cvCircle( img, center, cvRound(magnitude*1.2), color, 3, CV_AA, 0 );
        cvLine( img, center, cvPoint( cvRound( center.x + magnitude*cos(angle*CV_PI/180)),
                cvRound( center.y - magnitude*sin(angle*CV_PI/180))), color, 3, CV_AA, 0 );

    }

    Mat img_mat = cv::cvarrToMat(img);
    Mat dst_mat = cv::cvarrToMat(dst);

    drawObjects( img_mat, dst_mat);

    drawPoints(img, points);

    if (points.size() > MAX_OBJECT_TRACK_POINT_COUNT) {
        points.clear();
    }

}

int main(int argc, char** argv)
{

    IplImage* motion = 0;
    CvCapture* capture = 0;

    if( argc == 2 ) {
        std::cout <<  "Capture file:" << argv[1] << std::endl;
        capture = cvCaptureFromFile(argv[1]);
        if (!capture) {
            std::cout << "capture is null." << std::endl;
        }
    } else {
        std::cout << "Error there are no 2 arg." << std::endl;
    }

    if( capture )
    {
        cvNamedWindow( "Motion", 1 );

        for(;;)
        {
            IplImage* image = cvQueryFrame( capture );
            if( !image ) {
                break;
            }

            if( !motion )
            {
                std::cout << "motion" << std::endl;
                motion = cvCreateImage( cvSize(image->width,image->height), IPL_DEPTH_8U, 1 );
                cvZero( motion );
                motion->origin = image->origin;
            }

            update_mhi( image, motion, 15 );
            cvShowImage( "Motion", motion );
            cvShowImage("CurFrame", image);
            if( cvWaitKey(10) >= 0 )
                break;
        }
        cvReleaseCapture( &capture );
        cvDestroyWindow( "Motion" );
        cvDestroyWindow( "CurFrame" );
    }

    return 0;
}
