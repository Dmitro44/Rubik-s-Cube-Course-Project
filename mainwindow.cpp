#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Rubiks Cube");

    // Create the OpenGL widget
    openGLWidget = new OpenGLWidget(this);
    ui->gridLayout->addWidget(openGLWidget);
    openGLWidget->setFocusPolicy(Qt::StrongFocus);
}

MainWindow::~MainWindow()
{
    delete ui;
}
