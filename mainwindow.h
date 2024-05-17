#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTime>

#include "openglwidget.h"
#include "solcubdialog.h"
#include "history.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void cubeSolved();

    void startTimer();

    void updateTimer();

    void closeDialog();

    void showHistory();

    void saveSolutionToHistory();

private:
    Ui::MainWindow *ui;
    OpenGLWidget *openGLWidget;
    SolCubDialog *solCubDialog;
    History *history;
    QTimer *timer;
    QTime *stopwatchTime;
    QString stopTime;
};
#endif // MAINWINDOW_H
