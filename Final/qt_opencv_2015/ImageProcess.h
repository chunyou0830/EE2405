#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H

#include "ui_subwindow.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <QThread>
#include <QWidget>
#include "xbee.h"

using namespace cv;
using namespace std;

class ImageProcess : public QThread {
    Q_OBJECT

public:
    ImageProcess(QObject *parent);
    ~ImageProcess();

public slots:
    void processImage(cv::Mat &image);
    void changeToNormal();
    void changeToEdgeDetection();
    void changeToCircleDetection();
    void changeToSquareDetection();
    void changeToLineDetection();

signals:

protected:
    QWidget *subWidget;

    void run();

private:
    Ui::SubWindow ui;
    int method;

    QImage imageConvert(cv::Mat &matImage);
    cv::Mat edgeDetection(cv::Mat &image);
    cv::Mat findCircles(cv::Mat &image);
    cv::Mat findLines(cv::Mat &image);
    cv::Mat roadLanDetection(cv::Mat &image);
    void sendXbee(char content);
    void findSquares(cv::Mat &image, vector<vector<Point> > &squares);
    void drawSquares(cv::Mat &image, vector<vector<Point> > &squares);
    double angle(Point pt1, Point pt2, Point pt0);
    Xbee *xbee;

};

#endif // IMAGEPROCESS_H
