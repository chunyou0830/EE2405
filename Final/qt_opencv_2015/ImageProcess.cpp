#include "ImageProcess.h"
#include "mainWindow.h"
#include "xbee.h"
#include "ui_subwindow.h"
#include <QLabel>
#include <QDebug>

#define PI 3.1415926

using namespace cv;

ImageProcess::ImageProcess(QObject *parent) : QThread(parent) {
    subWidget = new QWidget();
    ui.setupUi(subWidget);

    method = 0;

    subWidget->show();
    xbee = new Xbee("/dev/ttyUSB0");
}

ImageProcess::~ImageProcess() {
    delete this;
}

void ImageProcess::run() {

    qDebug() << "New thread started successfully!!";

}

void ImageProcess::sendXbee(char content){
    xbee->setDir(content);
    xbee->start();
}

void ImageProcess::processImage(cv::Mat &image) {
    cv::Mat process;
    vector<vector<Point> > squares;

    // Determine how to process the image
    process = edgeDetection(image);

    // Display processed image to another widget.
    QPixmap pix = QPixmap::fromImage(imageConvert(process));
    ui.displayLabel->setPixmap(pix);

    // Free the memory to avoid overflow.
    image.release();
}

cv::Mat ImageProcess::edgeDetection(cv::Mat &image) {
    int houghVote = 200;
    cvtColor(image,image,CV_RGB2GRAY);

    // Canny algorithm
    Mat contours;
    Canny(image,contours,50,250);
    Mat contoursInv;
    threshold(contours,contoursInv,128,255,THRESH_BINARY_INV);

    // Lines detection
    std::vector<Vec2f> lines;
    if (houghVote < 1 or lines.size() > 2){ // we lost all lines. reset
        houghVote = 200;
    }
    else{ houghVote += 25;}
    while(lines.size() < 5 && houghVote > 0){
        HoughLines(contours,lines,1,PI/180, houghVote);
        houghVote -= 5;
    }
    Mat result(image.size(),CV_8U,Scalar(255));
    image.copyTo(result);

    // Draw the limes
    std::vector<Vec2f>::const_iterator it= lines.begin();
    //Mat hough(image.size(),CV_8U,Scalar(0));
    float top_avg = 0;
    int top_cnt = 0;
    while (it!=lines.end()) {

        float rho= (*it)[0];
        float theta= (*it)[1];
        if(theta > 0.09 && theta < 1.48 || theta < 3.14 && theta > 1.66){
            Point pt1(rho/cos(theta),0);
            Point pt2(rho-result.rows*sin(theta)/cos(theta),result.rows);
            float top = rho/cos(theta);
            if(top < 220 && top > 100){
                top_avg += top;
                top_cnt++;
                line(result, pt1, pt2, Scalar(0,0,255), 3, 8);
            }
        }
        ++it;
    }
    top_avg /= top_cnt;

    if(top_avg >= 140 && top_avg <= 180){
        qDebug() << "C";
        xbee->setDir('c');
        xbee->start();
    }
    else if(top_avg < 140){
        qDebug() << "L";
        xbee->setDir('l');
        xbee->start();
    }
    else if(top_avg > 180){
        qDebug() << "R";
        xbee->setDir('r');
        xbee->start();
    }
    else{
        qDebug() << "Err";
        xbee->setDir('e');
        xbee->start();
    }

    return result;
}

void ImageProcess::findSquares(cv::Mat &image, vector<vector<Point> > &squares) {
    squares.clear();

    Mat pyr, timg, gray0(image.size(), CV_8U), gray;
    int thresh = 50, N = 11;

    // down-scale and upscale the image to filter out the noise
    pyrDown(image, pyr, Size(image.cols/2, image.rows/2));
    pyrUp(pyr, timg, image.size());
    vector<vector<Point> > contours;

    // find squares in every color plane of the image
    for( int c = 0; c < 3; c++ )
    {
        int ch[] = {c, 0};
        cv::mixChannels(&timg, 1, &gray0, 1, ch, 1);

        // try several threshold levels
        for( int l = 0; l < N; l++ )
        {
            // hack: use Canny instead of zero threshold level.
            // Canny helps to catch squares with gradient shading
            if( l == 0 )
            {
                // apply Canny. Take the upper threshold from slider
                // and set the lower to 0 (which forces edges merging)
                cv::Canny(gray0, gray, 0, thresh, 5);
                // dilate canny output to remove potential
                // holes between edge segments
                cv::dilate(gray, gray, Mat(), Point(-1,-1));
            }
            else
            {
                // apply threshold if l!=0:
                // tgray(x,y) = gray(x,y) < (l+1)*255/N ? 255 : 0
                gray = gray0 >= (l+1)*255/N;
            }

            // find contours and store them all as a list
            cv::findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

            vector<Point> approx;

            // test each contour
            for( size_t i = 0; i < contours.size(); i++ )
            {
                // approximate contour with accuracy proportional
                // to the contour perimeter
                cv::approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

                // square contours should have 4 vertices after approximation
                // relatively large area (to filter out noisy contours)
                // and be convex.
                // Note: absolute value of an area is used because
                // area may be positive or negative - in accordance with the
                // contour orientation
                if( approx.size() == 4 &&
                        fabs(contourArea(Mat(approx))) > 1000 &&
                        cv::isContourConvex(Mat(approx)) )
                {
                    double maxCosine = 0;

                    for( int j = 2; j < 5; j++ )
                    {
                        // find the maximum cosine of the angle between joint edges
                        double cosine = fabs(angle(approx[j%4], approx[j-2], approx[j-1]));
                        maxCosine = MAX(maxCosine, cosine);
                    }

                    // if cosines of all angles are small
                    // (all angles are ~90 degree) then write quandrange
                    // vertices to resultant sequence
                    if( maxCosine < 0.3 )
                        squares.push_back(approx);
                }
            }
        }
    }
}

void ImageProcess::drawSquares(cv::Mat &image, vector<vector<Point> > &squares) {
    // the function draws all the squares in the image
    for( size_t i = 0; i < squares.size(); i++ ) {
        const Point* p = &squares[i][0];
        int n = (int)squares[i].size();

        polylines(image, &p, &n, 1, true, Scalar(0,255,0), 3, CV_AA);
    }
}

cv::Mat ImageProcess::findCircles(cv::Mat &image) {
    cv::Mat circle;
    vector<Vec3f> circles;

    // Convert it to gray
    cvtColor(image, circle, CV_BGR2GRAY);

    // Reduce the noise so we avoid false circle detection
    GaussianBlur(circle, circle, Size(9, 9), 2, 2);

    // Apply the Hough Transform to find the circles
    HoughCircles(circle, circles, CV_HOUGH_GRADIENT, 1, circle.rows/16, 150, 30, 0, 0);

    // Draw the circles detected
    for(size_t i = 0; i < circles.size(); i++ ) {
        cv::Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
        int radius = cvRound(circles[i][2]);

        // circle center
        cv::circle(image, center, 3, cv::Scalar(0,255,0), -1, 8, 0 );
        // circle outline
        cv::circle(image, center, radius, cv::Scalar(0,0,255), 3, 8, 0 );
    }

    return image;
}

cv::Mat ImageProcess::findLines(cv::Mat &image) {
    cv::Mat line;
    vector<Vec4i> lines;

    // Convert it to gray
    cvtColor(image, line, CV_BGR2GRAY);

    // Detect the edges of the image by using a Canny detector
    Canny(line, line, 50, 200, 3);

    // Apply the Hough Transform to find the line segments
    HoughLinesP(line, lines, 1, CV_PI/180, 50, 50, 10);

    // Draw the line segments detected
    for(size_t i = 0; i < lines.size(); i++) {
        Vec4i l = lines[i];

        cv::line(image, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
    }

    return image;
}

// helper function:
// finds a cosine of angle between vectors
// from pt0->pt1 and from pt0->pt2
double ImageProcess::angle(Point pt1, Point pt2, Point pt0)
{
    double dx1 = pt1.x - pt0.x;
    double dy1 = pt1.y - pt0.y;
    double dx2 = pt2.x - pt0.x;
    double dy2 = pt2.y - pt0.y;
    return (dx1*dx2 + dy1*dy2)/sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

// The following slots are used to set image processing mode.
void ImageProcess::changeToNormal() {
    method = 1;
}

void ImageProcess::changeToEdgeDetection() {
    method = 1;
}

void ImageProcess::changeToCircleDetection() {
    method = 1;
}

void ImageProcess::changeToSquareDetection() {
    method = 1;
}

void ImageProcess::changeToLineDetection() {
    method = 1;
}

QImage ImageProcess::imageConvert(cv::Mat &matImage) {
    QImage::Format format;

    // If we use edge detection, we will use gray scale to display image.
    switch(method) {
    case 1: format = QImage::Format_Indexed8;
        break;
    default:format = QImage::Format_RGB888;
        break;
    }

    // Convert processed openCV frame to Qt's image format in order to display.
    QImage qImage(
                (uchar*)matImage.data,
                matImage.cols,
                matImage.rows,
                matImage.step,
                format
                );

    return qImage.rgbSwapped().mirrored(false, false);
}
