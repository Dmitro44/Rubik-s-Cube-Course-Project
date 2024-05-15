#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLShader>
#include <QOpenGLContext>
#include <QMatrix4x4>
#include <QQuaternion>
#include <QMouseEvent>
#include <QKeyEvent>

#include "rubikscube.h"

#define vec3Cube QVector<QVector<QVector<CubeGeometry>>> // 3x3x3 cube

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{

public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget() override;
protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void setupCamera();

    void updateRotationSideAxises(QVector3D rotationAroundAxis, bool clockwise);

private:
    RubiksCube *rubiksCube;

    QVector3D cameraPos;
    QVector3D cameraFront;
    QVector3D cameraUp;

    QMatrix4x4 projection;
    QMatrix4x4 model;
    QMatrix4x4 view;

    // float yaw = 270.0f;
    float yaw = 225.0f;
    float pitch = -35.0f;
    // float pitch = 0.0f;

    QPoint lastMousePos;
    bool rightButtonPressed = false;
    bool leftButtonPressed = false;

    QVector3D rotationUpDownSideAxis;
    QVector3D rotationLeftRightSideAxis;
    QVector3D rotationFrontBackSideAxis;

    QQuaternion targetOrientation;
    QQuaternion currentOrientation;
    QQuaternion targetRotation;
    QQuaternion currentRotation;
};

#endif // OPENGLWIDGET_H
