// Copyright (C) 2014 Micah Yoder
// Copying subject to the GNU GPL v3

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPoint>
#include <QPainter>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    HD(1920, 1080)
{
    ui->setupUi(this);

    // set up various calculations
    framesPerPhase << 0 << 30 << 140 << 80;  // first is a dummy for one-based indexing
    shrinkPerFrame = (1-shrinkFactor) / framesPerPhase[3];
    rotatePerFrame = rotateTotal/framesPerPhase[3];
    // the numbers in the following lines are fudge factors that I got by trial and error;
    // they are not trivial due to rotation screwing things up (even though I thought I was rotating around the center)
    transXperFrame1 = 700/framesPerPhase[3]; // shrinking image from full size to its position on the final background
    transXperFrame2 = 870/framesPerPhase[3];  // kind of unintuitive, different X offsets needed because rotation wacked things up
    transYperFrame1 = 100/framesPerPhase[3]; // land at different Y points for even and odd pics
    transYperFrame2 = 400/framesPerPhase[3];
    workBGshiftPerFrame = HD.width()*shrinkFactor / (framesPerPhase[1] + framesPerPhase[2] + framesPerPhase[3]);

    // set up images
    background = QImage(HD, QImage::Format_RGB32);
    background.fill(0);  // "real" background is black
    workBackground = QImage(HD, QImage::Format_ARGB32);
    workBackground.fill(0);  // "work" background starts transparent. We'll paste photos to this as they're done with their rotation, and shift left every frame.


    // set up timer to start processing a frame ASAP
    runItTimer = new QTimer(this);
    connect(runItTimer, SIGNAL(timeout()), this, SLOT(doNextFrame()));
    runItTimer->setSingleShot(true);
    runItTimer->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::renderFrame()
{
    QPoint translateOrigin;

    framePhase++;
    finalFrame = background;  // we start building the final frame by copying it from the "real" background
    // and then copy the current work background on top of it
    finalFrame = createImageWithOverlay(finalFrame, workBackground, QPoint(0,0));

    if (phase == 1 && framePhase == 1) {
        curPhotoOrig.load(photos.at(photo));
        // do we need to specify that it should have an alpha layer?
        fullsizecopy = curPhotoOrig;
        fullsizecopy = fullsizecopy.scaled(HD, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        xOffset = HD.width()-fullsizecopy.width();
    }

    if (phase == 1) {
        finalFrame = createImageWithOverlay(finalFrame, fullsizecopy, QPoint(xOffset,0), qreal(framePhase)/qreal(framesPerPhase[1]));
    }

    if (phase == 2) {
        finalFrame = createImageWithOverlay(finalFrame, fullsizecopy, QPoint(xOffset,0));
    }

    if (phase == 3) {
        currentShrinkFactor -= shrinkPerFrame;
        currentRotate += rotatePerFrame;
        translateX += photo % 2 ? transXperFrame2 : transXperFrame1;
        translateY += photo % 2 ? transYperFrame1 : transYperFrame2;
        image = curPhotoOrig;
        image = image.scaled(HD * currentShrinkFactor, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        translateOrigin = QPoint(translateX, translateY);
        // half the photos should rotate right and be moved up, other half rotated left and moved down
        finalFrame = createImageWithOverlay(finalFrame, image, translateOrigin, 1, photo % 2 ? currentRotate : (360-currentRotate) );

        // at the end of one photo, we "seal" it to the work background so it can be scrolled
        if (framePhase == framesPerPhase[3])
            workBackground = createImageWithOverlay(workBackground, image, QPoint(translateX, translateY));
    }

    // now write the frame
    QString filename = QString("%1/%2.png").arg(destDir).arg(frame, 8, 10, QChar('0'));
    finalFrame.save(filename);
    ui->photoWidget->setPixmap(QPixmap::fromImage(finalFrame));

    // shift the work background
    workBackground = workBackground.copy(QRect(QPoint(workBGshiftPerFrame, 0), HD));

    // other cleanup
    if (framePhase == framesPerPhase[phase]) {
        phase++;
        if (phase == 4) {
            phase = 1;
            photo++;
            if (photo == photos.size())
                return false;
            workBackground = finalFrame;
        }
        framePhase = 0;
        if (phase == 3) {
            currentShrinkFactor = 1.0;
            currentRotate = 0.0;
            translateX = xOffset;
            translateY = 0;
        }
    }
    return true;
}

void MainWindow::doNextFrame()
{
    if (renderFrame()) {
        frame++;
        runItTimer->start();
    } else
        close();
    QString statusmsg = QString("Rendering photo %1 frame %2").arg(photo).arg(frame);
    ui->statusBar->showMessage(statusmsg, 5000);
}

// copied from here, adding the orig, opacity, and rotate parameters:
// http://www.idevelopsoftware.com/2008/03/use-qimage-to-create-a-composite-image-ie-one-image-with-another-overlaid-on-top-of-it/
// see also:
// http://qt-project.org/doc/qt-5.1/qtwidgets/painting-imagecomposition.html

QImage MainWindow::createImageWithOverlay(const QImage& baseImage, const QImage& overlayImage, QPoint orig, qreal opacity, qreal rotate)
{
    QImage imageWithOverlay = QImage(baseImage.size(), QImage::Format_RGB32); // was  Format_ARGB32_Premultiplied
    QPainter painter(&imageWithOverlay);

    qint32 halfX = overlayImage.size().width() / 2;
    qint32 halfY = overlayImage.size().height() / 2;

    painter.setCompositionMode(QPainter::CompositionMode_Source);
    painter.fillRect(imageWithOverlay.rect(), Qt::transparent);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.drawImage(0, 0, baseImage);

    painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter.setOpacity(opacity);
    if (abs(rotate) > 0.1) {
        painter.translate(halfX, halfY);
        painter.rotate(rotate);
        painter.drawImage(orig.x() - halfX, orig.y() - halfY, overlayImage);
    } else
        painter.drawImage(orig.x(), orig.y(), overlayImage);

    painter.end();

    return imageWithOverlay;
}
