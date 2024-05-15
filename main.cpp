#include "mainwindow.h"

#include <QApplication>
#include <QSurfaceFormat>

#include "openglwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QSurfaceFormat format;
    format.setDepthBufferSize(24);
    QSurfaceFormat::setDefaultFormat(format);

    MainWindow w;
    w.show();
    return a.exec();
}
