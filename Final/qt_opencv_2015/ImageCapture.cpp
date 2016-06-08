#include "ImageCapture.h"
#include <QImage>
#include <QDebug>

ImageCapture::ImageCapture(QWidget *parent) : QWidget(parent) {
    cvCaptureTimer = new QTimer(this);
    counter = 0;
    threshold = 0;
    mode = 0;

    // Connect signal to slot.
    connect(cvCaptureTimer, SIGNAL(timeout()), this, SLOT(timerTick()));
}

ImageCapture::~ImageCapture() {
    delete this;
}

QImage ImageCapture::imageConvert(cv::Mat &matImage) {

    // Convert incoming openCV frame to Qt's image format in order to display.
    QImage qImage(
        (uchar*)matImage.data,
        matImage.cols,
        matImage.rows,
        matImage.step,
        QImage::Format_RGB888
        );

    return qImage.rgbSwapped().mirrored(false, false);
}

void ImageCapture::captureFromCamera(int cameraIndex) {
    cvCapture = new cv::VideoCapture("http://192.168.1.6:8080/?action=stream.mjpeg");

    // Use openCV to capture frame from camera
    if(cvCapture->isOpened()) {
        qDebug() << "Camera opened sucessfully!!!";

        // Maintain a Qt-timer to set the time interval for cpaturing each frame from camera.
        cvCaptureTimer->setInterval(50);
        cvCaptureTimer->start();
    } else {
        qDebug() << "Could not open camera at cameraIndex:" << cameraIndex;
    }
}

void ImageCapture::timerTick() {
    cv::Mat frame;

    // Capture a frame from camera
    cvCapture->read(frame);

    // Check whether frame data are available
    if(!frame.empty()) {
        QImage image(imageConvert(frame));

        // Check whether we are in video mode
        if(mode == 0) {
            // Maintain a counter to determine whether we will process incoming frame or not.
            if(counter >= threshold) {
                counter = 0;
                emit processImage(frame);
            } else {
                counter++;
            }
        }

        emit imageCaptured(image);
    } else {
        qDebug() << "Fail to read the frames from the cvCapture!!";
    }
}

void ImageCapture::setDelayTo50ms() {
    threshold = 0;
}

void ImageCapture::setDelayTo250ms() {
    threshold = 5;
}

void ImageCapture::setDelayTo500ms() {
    threshold = 10;
}

void ImageCapture::setDelayTo1s() {
    threshold = 20;
}

void ImageCapture::setModeToVideo() {
    mode = 0;
}

void ImageCapture::setModeToImage() {
    mode = 1;
}

void ImageCapture::captureOneFrame() {
    // Check whether we are in image mode
    if(mode == 1) {
        cv::Mat frame;

        cvCapture->read(frame);
        emit processImage(frame);
    }
}
