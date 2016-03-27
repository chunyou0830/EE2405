#include <opencv2/opencv.hpp>
#include <iostream>
#include "cv.h"
#include "highgui.h"

using namespace cv;
using namespace std;

int edgeThresh = 50;
void main(){
	Mat srcImg = imread("D:/book.jpg", CV_LOAD_IMAGE_UNCHANGED);
	blur(srcImg, srcImg, Size(10, 10));
	IplImage* pSrcImg = &IplImage(srcImg);
	IplImage* pDstImg = cvCreateImage(cvGetSize(pSrcImg), 8, 1);
	IplImage* pColorImg = cvCreateImage(cvGetSize(pSrcImg), 8, 3);
	CvMemStorage* storage = cvCreateMemStorage(0);

	Mat matImgTst = cvarrToMat(pSrcImg);

	CvSeq* lines = 0;

	// Canny Edge Detection
	cvCanny(pSrcImg, pDstImg, 100, 100, 3);

	// Hough Lines Detection
	lines = cvHoughLines2(pDstImg, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 100, 100, 100);

	// Draw lines
	for(int i = 0; i < lines->total; i++) {
		CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
		cvLine(pSrcImg, line[0], line[1], CV_RGB(255, 0, 0), 3, CV_AA, 0);
		cv::Scalar c = cv::Scalar(0, 255, 0);
		cout << i << ": (" << line[0].x << "," << line[0].y << ") (" << line[1].x << "," << line[1].y << ")" << endl;
		//col += 30;
	}
	// Show images
	cvShowImage("Source", pSrcImg);
	cvWaitKey(0);
}