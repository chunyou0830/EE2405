#include <opencv2/opencv.hpp>
#include <iostream>
#include "cv.h"
#include "highgui.h"
/*
int main()
{	
	cv::Mat srcImg = cv::imread("D:/eenight.jpg", CV_LOAD_IMAGE_UNCHANGED);
	cv::Mat resizeImg;
	cv::Mat grayImg;

	if (!srcImg.data) {
		std::cout << "Could not open or find the image" << std::endl;
		return -1;
	}
	cv::resize(srcImg, resizeImg, cv::Size(srcImg.cols / 2, srcImg.rows / 2));
	cv::cvtColor(resizeImg, grayImg, CV_RGB2GRAY);

	cv::imshow("Image Display", srcImg);
	cv::imshow("Gray Image Display", grayImg);
	cv::waitKey(0);

	return 0;
}*/

int edgeThresh = 50;
void main(){
	IplImage* pSrcImg = cvLoadImage("D:/park.jpg", 0);
	IplImage* pDstImg = cvCreateImage(cvGetSize(pSrcImg), 8, 1);
	IplImage* pColorImg = cvCreateImage(cvGetSize(pSrcImg), 8, 3);
	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* lines = 0;

	// Canny Edge Detection
	cvCanny(pSrcImg, pDstImg, 50, 150, 3);
	
	// CV_BGR2GRAY Grayscale Conversion
	cvCvtColor(pDstImg, pColorImg, CV_GRAY2BGR);

	// Hough Lines Detection
	lines = cvHoughLines2(pDstImg, storage, CV_HOUGH_PROBABILISTIC, 1, CV_PI / 180, 50, 50, 10);

	// Draw lines
	for (int i = 0; i < lines->total; i++){
		CvPoint* line = (CvPoint*)cvGetSeqElem(lines, i);
		cvLine(pColorImg, line[0], line[1], CV_RGB(255, 0, 0), 3, CV_AA, 0);
	}

	// Show images
	cvShowImage("Source", pSrcImg);
	cvShowImage("Hough", pColorImg);
	cvWaitKey(0);
}