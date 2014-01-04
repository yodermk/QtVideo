// Copyright (C) 2014 Micah Yoder
// Copying subject to the GNU GPL v3

#include "mainwindow.h"
#include <QApplication>
#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QStringList>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QCommandLineParser parser;
    parser.setApplicationDescription("Convert photos to video");
    parser.addHelpOption();
    QCommandLineOption targetDirectory(QStringList() << "d" << "destination-directory", "Directory in which to place the video files", "dest", ".");
    parser.addOption(targetDirectory);
    parser.setApplicationDescription("Converts a list of photos on the command line to frames which ffmpeg can turn into a nice video.\nQuick & dirty - assumptions are made so don't try to abuse me!");
    parser.process(a);
    w.photos = parser.positionalArguments();
    w.destDir = parser.value(targetDirectory);

    w.show();

    return a.exec();
}
