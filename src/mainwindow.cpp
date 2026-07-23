#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Rubiks Cube");
    setWindowIcon(QIcon(":/icons/Cube_icon.ico"));

    QPalette pal = this->palette();
    pal.setColor(QPalette::Window, QColor(179,255,179));
    this->setPalette(pal);

    // Create the OpenGL widget
    openGLWidget = new OpenGLWidget(this);
    solCubDialog = new SolCubDialog(this);
    history = new History(this);

    ui->gridForGL->addWidget(openGLWidget);
    openGLWidget->setFocusPolicy(Qt::StrongFocus);
    openGLWidget->setFocus();

    timer = new QTimer(this);
    stopwatchTime = new QTime(0, 0);

    connect(openGLWidget->getRubiksCube(), SIGNAL(cubeSolved()), this, SLOT(cubeSolved()));
    connect(openGLWidget, SIGNAL(firstMove()), this, SLOT(startTimer()));
    connect(timer, SIGNAL(timeout()), this, SLOT(updateTimer()));

    connect(solCubDialog, SIGNAL(SaveSolution()), this, SLOT(saveSolutionToHistory()));
    connect(solCubDialog, SIGNAL(CancelSolution()), this, SLOT(closeDialog()));

    connect(ui->history_button, SIGNAL(clicked()), this, SLOT(showHistory()));

    connect(ui->scramble_button, SIGNAL(clicked()), openGLWidget, SLOT(updateScramble()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete openGLWidget;
    delete solCubDialog;
    delete timer;
    delete stopwatchTime;
}

void MainWindow::cubeSolved()
{
    timer->stop();
    stopTime = stopwatchTime->toString("mm:ss");

    openGLWidget->setFirstMoveFlag(false);

    solCubDialog->show();
}

void MainWindow::startTimer()
{

    ui->timer_label->setText(stopwatchTime->toString("mm:ss"));
    timer->start(1000);
}

void MainWindow::updateTimer()
{
    *stopwatchTime = stopwatchTime->addSecs(1);
    ui->timer_label->setText(stopwatchTime->toString("mm:ss"));
}

void MainWindow::closeDialog()
{
    solCubDialog->close();
    stopwatchTime->setHMS(0, 0, 0);
    ui->timer_label->setText(stopwatchTime->toString("mm:ss"));
}

void MainWindow::showHistory()
{
    history->showHistory();
    history->show();
}

void MainWindow::saveSolutionToHistory()
{
    stopwatchTime->setHMS(0, 0, 0);
    ui->timer_label->setText(stopwatchTime->toString("mm:ss"));
    solCubDialog->close();
    history->addInfoToFile(stopTime, openGLWidget->getRubiksCube()->getScramble(), openGLWidget->getRubiksCube()->getSolution());
    openGLWidget->getRubiksCube()->getScramble().clear();
    openGLWidget->getRubiksCube()->getSolution().clear();
}
