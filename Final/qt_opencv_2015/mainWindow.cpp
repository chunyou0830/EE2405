#include "mainWindow.h"


mainWindow::mainWindow(QMainWindow *parent) : QMainWindow(parent), mainWindow_ui(new Ui::MainWindow)
{
    mainWindow_ui->setupUi(this);

    //xbee = new Xbee("/dev/ttyUSB0");

    // Setup toolbar items
    mainWindow_ui->mainToolBar->addSeparator();
    actionQuit = mainWindow_ui->mainToolBar->addAction("Quit");
    mainWindow_ui->mainToolBar->addSeparator();
    actionNormalCapture = mainWindow_ui->mainToolBar->addAction("Normal");
    actionEdgeDetection = mainWindow_ui->mainToolBar->addAction("Edge");
    actionCircleDetection = mainWindow_ui->mainToolBar->addAction("Circle");
    actionSquareDetection = mainWindow_ui->mainToolBar->addAction("Square");
    actionLineDetection = mainWindow_ui->mainToolBar->addAction("Line");
    mainWindow_ui->mainToolBar->addSeparator();

    // Setup delay menu
    selectDelayMenu = new QMenu(this);
    actionDelay50ms = new QAction("50ms", this);
    actionDelay250ms = new QAction("250ms", this);
    actionDelay500ms = new QAction("500ms", this);
    actionDelay1s = new QAction("1s", this);
    selectDelayMenu->addAction(actionDelay50ms);
    selectDelayMenu->addAction(actionDelay250ms);
    selectDelayMenu->addAction(actionDelay500ms);
    selectDelayMenu->addAction(actionDelay1s);
    setDelayButton = new QToolButton(this);
    setDelayButton->setText("Delay");
    setDelayButton->setMenu(selectDelayMenu);
    setDelayButton->setPopupMode(QToolButton::InstantPopup);
    setDelayButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    mainWindow_ui->mainToolBar->addWidget(setDelayButton);

    // Setup mode menu
    selectModeMenu = new QMenu(this);
    actionModeVideo = new QAction("Video", this);
    actionModeImage = new QAction("Image", this);
    selectModeMenu->addAction(actionModeVideo);
    selectModeMenu->addAction(actionModeImage);
    setModeButton = new QToolButton(this);
    setModeButton->setText("Mode");
    setModeButton->setMenu(selectModeMenu);
    setModeButton->setPopupMode(QToolButton::InstantPopup);
    setModeButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
    mainWindow_ui->mainToolBar->addWidget(setModeButton);
    actionCaptureOneFrame = mainWindow_ui->mainToolBar->addAction("Capture");

    // Create a new ImageCpature class for image cpature.
    imageCapture = new ImageCapture(parent);
    imageCapture->captureFromCamera(-1);

    // Create a new ImageProcess class for image process.
    // This is a new separate thread.
    imageProcess = new ImageProcess(parent);
    imageProcess->start();

    // Connect signals to slots.
    connect(imageCapture, SIGNAL(imageCaptured(QImage &)), this, SLOT(displayImage(QImage &)));
    connect(imageCapture, SIGNAL(processImage(cv::Mat &)), imageProcess, SLOT(processImage(cv::Mat &)));
    connect(actionQuit, SIGNAL(triggered()), this, SLOT(deleteLater()));
    connect(actionNormalCapture, SIGNAL(triggered()), imageProcess, SLOT(changeToNormal()));
    connect(actionEdgeDetection, SIGNAL(triggered()), imageProcess, SLOT(changeToEdgeDetection()));
    connect(actionCircleDetection, SIGNAL(triggered()), imageProcess, SLOT(changeToCircleDetection()));
    connect(actionSquareDetection, SIGNAL(triggered()), imageProcess, SLOT(changeToSquareDetection()));
    connect(actionLineDetection, SIGNAL(triggered()), imageProcess, SLOT(changeToLineDetection()));
    connect(actionDelay50ms, SIGNAL(triggered()), imageCapture, SLOT(setDelayTo50ms()));
    connect(actionDelay250ms, SIGNAL(triggered()), imageCapture, SLOT(setDelayTo250ms()));
    connect(actionDelay500ms, SIGNAL(triggered()), imageCapture, SLOT(setDelayTo500ms()));
    connect(actionDelay1s, SIGNAL(triggered()), imageCapture, SLOT(setDelayTo1s()));
    connect(actionModeVideo, SIGNAL(triggered()), imageCapture, SLOT(setModeToVideo()));
    connect(actionModeImage, SIGNAL(triggered()), imageCapture, SLOT(setModeToImage()));
    connect(actionCaptureOneFrame, SIGNAL(triggered()), imageCapture, SLOT(captureOneFrame()));

}

mainWindow::~mainWindow()
{
    delete this;
}

void mainWindow::displayImage(QImage &image) {

    // Display captured image in displayLabel.
    mainWindow_ui->displayLabel->setPixmap(QPixmap::fromImage(image));
    mainWindow_ui->displayLabel->adjustSize();
}

void mainWindow::sendXbee(char content){
    xbee->setDir(content);
    xbee->start();
}

void mainWindow::on_pushButton_clicked()
{
    // Up
    /*xbee->setDir('f');
    qDebug() << "Send f";
    xbee->start();*/
}

void mainWindow::on_pushButton_4_clicked()
{
    // Down
    /*xbee->setDir('b');
    qDebug() << "Send b";
    xbee->start();*/
}

void mainWindow::on_pushButton_3_clicked()
{
    // Left
    /*xbee->setDir('l');
    qDebug() << "Send l";
    xbee->start();*/

}

void mainWindow::on_pushButton_2_clicked()
{
    // Right
    /*xbee->setDir('r');
    qDebug() << "Send r";
    xbee->start();*/
}
