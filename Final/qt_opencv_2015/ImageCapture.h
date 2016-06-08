#ifndef IMAGECPATURE_H
#define IMAGECPATURE_H

#include <QWidget>
#include <QTimer>
#include "opencv2/highgui/highgui.hpp"

using namespace cv;

class ImageCapture : public QWidget {
    Q_OBJECT

public:
    ImageCapture(QWidget *parent);
    ~ImageCapture();

public slots:
    void captureFromCamera(int cameraIndex=-1);
    void timerTick();
    void setDelayTo50ms();
    void setDelayTo250ms();
    void setDelayTo500ms();
    void setDelayTo1s();
    void setModeToVideo();
    void setModeToImage();
    void captureOneFrame();

signals:
    void imageCaptured(QImage &image);
    void processImage(cv::Mat &image);

protected:
    cv::VideoCapture *cvCapture;
    QTimer *cvCaptureTimer;
    int counter;
    int threshold;
    int mode;

    QImage imageConvert(Mat &matImage);
};

#endif // IMAGECPATURE_H
