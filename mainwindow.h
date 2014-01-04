// Copyright (C) 2014 Micah Yoder
// Copying subject to the GNU GPL v3

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QStringList>
#include <QSize>
#include <QImage>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    bool renderFrame();
    QStringList photos;
    QString destDir;

public slots:
    void doNextFrame();

private:
    Ui::MainWindow *ui;
    // QString imgdir = "/home/micah/photos/5D3/Hood/";
    QString destdir = "./";
    // QVector<QString> photos;
    QVector<int> framesPerPhase;
    double shrinkFactor = 0.5;
    double shrinkPerFrame;
    double rotateTotal = 20;  // degrees of final rotation
    double rotatePerFrame;
    int transXperFrame1;
    int transXperFrame2;
    int transYperFrame1;
    int transYperFrame2;
    int workBGshiftPerFrame;
    int xOffset;  // difference between full X resolution and X width of image
    QSize HD;
    QImage image;  // image we're currently working with
    QImage fullsizecopy; // so we don't have to do the same resize over and over
    QImage background;   // "real" background
    QImage workBackground;  // "save" space for rolling background with images
    QImage finalFrame;  // final frame written to disk
    QImage curPhotoOrig;  // current photo as read from disk
    int frame = 0;  // frame number in the sequence
    int photo = 0;   // photo number (index into photos)
    int phase = 1;   // which phase are we currently in
    int framePhase = 0;  // number of frames into the current phase
    double currentShrinkFactor;
    double currentRotate;
    int translateX, translateY;
    QTimer *runItTimer;

    QImage createImageWithOverlay(const QImage& baseImage, const QImage& overlayImage, QPoint orig, qreal opacity = 1.0, qreal rotate = 0.0);
};

#endif // MAINWINDOW_H
