#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ImageCapture.h"
#include "ImageProcess.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QMenu>
#include <QToolButton>
#include <QDebug>
#include <QThread>
#include "xbee.h"


class mainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    //explicit mainWindow(QMainWindow *parent = 0);
    explicit mainWindow(QMainWindow *parent = 0);
    ~mainWindow();

    void mainxbee(int argc, char *argv[]);
    void sendXbee(char content);


public slots:
    void displayImage(QImage &image);

protected:
    ImageCapture *imageCapture;
    ImageProcess *imageProcess;

private slots:
    void on_pushButton_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *mainWindow_ui;
    QMenu *selectDelayMenu;
    QMenu *selectModeMenu;
    QToolButton *setDelayButton;
    QToolButton *setModeButton;
    QAction *actionQuit;
    QAction *actionNormalCapture;
    QAction *actionEdgeDetection;
    QAction *actionCircleDetection;
    QAction *actionSquareDetection;
    QAction *actionLineDetection;
    QAction *actionDelay50ms;
    QAction *actionDelay250ms;
    QAction *actionDelay500ms;
    QAction *actionDelay1s;
    QAction *actionModeVideo;
    QAction *actionModeImage;
    QAction *actionCaptureOneFrame;
    Xbee *xbee;

};

#endif // MAINWINDOW_H
