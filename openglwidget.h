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

#include "cubegeometry.h"

enum class CubeFace {
    Front = 0,
    Back,
    Left,
    Right,
    Top,
    Bottom
};

struct CubeData {
    CubeGeometry cube;
    CubeFace face;
};

class OpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
public:
    explicit OpenGLWidget(QWidget *parent = nullptr);

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

    QVector3D getRayWorld(const QPoint& screenPos);
    void setElementsOfCube(QVector<CubeData> &cubes, QVector<QVector3D> &colors);

private:
    QVector<CubeData> cubes;
    QVector<QVector3D> colors;

    QVector3D cameraPos;
    QVector3D cameraFront;
    QVector3D cameraUp;

    QMatrix4x4 projection;
    QMatrix4x4 model;
    QMatrix4x4 view;

    float yaw = 225.0f;
    float pitch = -35.0f;

    QPoint lastMousePos;
    bool rightButtonPressed = false;
    bool leftButtonPressed = false;
    bool swipeStarted = false;
    QPoint swipeStartPos;
    QPoint swipeEndPos;
    QVector3D rotationAxis;
    QQuaternion targetOrientation;
    QQuaternion currentOrientation;
    //QQuaternion rotation;
};

#endif // OPENGLWIDGET_H
