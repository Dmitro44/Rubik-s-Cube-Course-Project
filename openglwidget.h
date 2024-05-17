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
#include <QObject>

#include "rubikscube.h"

#define vec3Cube QVector<QVector<QVector<CubeGeometry>>> // 3x3x3 cube

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT
public:
    explicit OpenGLWidget(QWidget *parent = nullptr);
    ~OpenGLWidget() override;

    RubiksCube *getRubiksCube() { return rubiksCube; }

    void setFirstMoveFlag(bool flag) { firstMoveFlag = flag; }

public slots:
    void updateScramble();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void focusOutEvent(QFocusEvent *event) override {
        QOpenGLWidget::focusOutEvent(event);
        setFocus();
    }
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void setupCamera();

    void updateRotationSideAxises(QVector3D rotationAroundAxis, bool clockwise);

signals:
    void firstMove();

private:
    RubiksCube *rubiksCube;

    bool firstMoveFlag = false;

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
