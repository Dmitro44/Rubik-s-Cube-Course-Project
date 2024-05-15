#include "openglwidget.h"
#include <QtMath>

constexpr float interpolationFactor = 0.05f;

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    rubiksCube = new RubiksCube();

    rotationFrontBackSideAxis = QVector3D(0.0f, 0.0f, 1.0f);
    rotationUpDownSideAxis = QVector3D(0.0f, 1.0f, 0.0f);
    rotationLeftRightSideAxis = QVector3D(1.0f, 0.0f, 0.0f);

    setupCamera();
}

OpenGLWidget::~OpenGLWidget()
{
    delete rubiksCube;
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    rubiksCube->setElementsOfCube();
    glClearColor(0.7f, 1.0f, 0.7f, 1.0f);
}

void OpenGLWidget::resizeGL(int w, int h)
{
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 0.1, far plane to 50.0, field of view 45 degrees
    const qreal zNear = 0.1, zFar = 50.0, fov = 45.0;

    // Reset projection
    projection.setToIdentity();

    // Set perspective projection
    projection.perspective(fov, aspect, zNear, zFar);
}

void OpenGLWidget::paintGL()
{
    // Clear color and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);

    currentOrientation = QQuaternion::slerp(currentOrientation, targetOrientation, interpolationFactor);

    // Calculate view transformation

    view.setToIdentity();
    QVector3D front(0.0f, 0.0f, 0.0f);
    front.setX(cos(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch)));
    front.setY(sin(qDegreesToRadians(pitch)));
    front.setZ(sin(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch)));
    cameraFront = front.normalized();

    view.lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Draw cube geometry
    vec3Cube &cubes = rubiksCube->getCubes();
    for (int x = 0; x < 3; ++x) {
        for (int y = 0; y < 3; ++y) {
            for (int z = 0; z < 3; ++z) {
                CubeGeometry &cube = cubes[x][y][z];
                model.setToIdentity();
                model.rotate(currentOrientation);

                cube.SetRotation(QQuaternion::slerp(cube.GetRotation(), cube.GetTargetRotation(), interpolationFactor * 2));
                model.rotate(cube.GetRotation());

                model.translate(cube.GetPosition());
                cube.SetModel(model);

                cube.drawCubeGeometry(projection, view);
            }
        }
    }
    update();
}

void OpenGLWidget::setupCamera()
{
    // Camera setup
    cameraPos = QVector3D(3.0f, 3.0f, 3.0f);
    cameraFront = QVector3D(-0.525f, -0.525f, 0.525f) - cameraPos;
    cameraFront.normalize();
    cameraUp = QVector3D(0.0f, 1.0f, 0.0f);
}

void OpenGLWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        lastMousePos = event->pos();
        rightButtonPressed = true;
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        rightButtonPressed = false;
    }
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (rightButtonPressed) {
        // Calculate the offset of the mouse movement
        float xoffset = lastMousePos.x() - event->x();
        float yoffset = lastMousePos.y() - event->y(); // reversed since y-coordinates go from bottom to top

        // Update last X and Y position
        lastMousePos = event->pos();

        float sensitivity = 0.2f; // Adjust the sensitivity as you like
        xoffset *= sensitivity;
        yoffset *= sensitivity;

        // Calculate the total mouse movement
        float totalMovement = sqrt(xoffset * xoffset + yoffset * yoffset);

        // If the mouse has moved a certain distance, rotate the cube by 90 degrees
        if (totalMovement > 1.0f) {
            float angle;
            QVector3D rotationAxis;
            bool clockwise;

            if (abs(xoffset) > abs(yoffset)) {
                rotationAxis = QVector3D(0.0f, 1.0f, 0.0f);
                if (xoffset > 0) {
                    angle = -90.0f;
                    clockwise = true;
                } else {
                    angle = 90.0f;
                    clockwise = false;
                }
            } else {
                if (event->x() < width() / 2)
                {
                    rotationAxis = QVector3D(1.0f, 0.0f, 0.0f);
                } else {
                    rotationAxis = QVector3D(0.0f, 0.0f, 1.0f);
                    yoffset = -yoffset;
                }
                // angle = yoffset > 0 ? -90.0f : 90.0f;
                if (yoffset > 0) {
                    angle = -90.0f;
                    clockwise = true;
                } else {
                    angle = 90.0f;
                    clockwise = false;
                }
            }
            updateRotationSideAxises(rotationAxis, clockwise);
            rubiksCube->rotateAllCubes(rotationAxis, clockwise);

            QQuaternion rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angle);

            targetOrientation = rotation.normalized() * targetOrientation;

            rightButtonPressed = false;
        }
    }
}

void OpenGLWidget::updateRotationSideAxises(QVector3D rotationAroundAxis, bool clockwise)
{
    QVector3D tempRotationAxis;
    if (rotationAroundAxis.x() != 0.0f) {
        tempRotationAxis = rotationUpDownSideAxis;
        rotationUpDownSideAxis = rotationFrontBackSideAxis;
        rotationFrontBackSideAxis = tempRotationAxis;

        if (!clockwise) {
            rotationUpDownSideAxis = -rotationUpDownSideAxis;
        } else {
            rotationFrontBackSideAxis = -rotationFrontBackSideAxis;
        }

    } else if (rotationAroundAxis.y() != 0.0f) {
        tempRotationAxis = rotationFrontBackSideAxis;
        rotationFrontBackSideAxis = rotationLeftRightSideAxis;
        rotationLeftRightSideAxis = tempRotationAxis;

        if (!clockwise) {
            rotationFrontBackSideAxis = -rotationFrontBackSideAxis;
        } else {
            rotationLeftRightSideAxis = -rotationLeftRightSideAxis;
        }

    } else {
        tempRotationAxis = rotationUpDownSideAxis;
        rotationUpDownSideAxis = rotationLeftRightSideAxis;
        rotationLeftRightSideAxis = tempRotationAxis;

        if (!clockwise) {
            rotationLeftRightSideAxis = -rotationLeftRightSideAxis;
        } else {
            rotationUpDownSideAxis = -rotationUpDownSideAxis;
        }
    }
}


void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    // Perform rotation of the side of the cube based on the key pressed
    QQuaternion rotation;
    bool clockwise = true;
    QVector<CubeGeometry *> cubesOnSide;
    switch (event->key()) {
        case Qt::Key_W:
            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(rotationUpDownSideAxis, 90.0f);
                clockwise = false;
            } else {
                rotation = QQuaternion::fromAxisAndAngle(rotationUpDownSideAxis, -90.0f);
                clockwise = true;
            }

            cubesOnSide = rubiksCube->getCubesOnSide('U');
            for (auto &cube : cubesOnSide) {
                cube->rotateCube(rotation);
            }
            rubiksCube->updateCubesAfterRotation('U', clockwise);
            break;
        case Qt::Key_S:
            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(rotationUpDownSideAxis, -90.0f);
                clockwise = false;
            } else {
                rotation = QQuaternion::fromAxisAndAngle(rotationUpDownSideAxis, 90.0f);
                clockwise = true;
            }

            cubesOnSide = rubiksCube->getCubesOnSide('D');
            for (auto &cube : cubesOnSide) {
                cube->rotateCube(rotation);
            }
            rubiksCube->updateCubesAfterRotation('D', clockwise);
            break;
        case Qt::Key_A:
            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(rotationLeftRightSideAxis, -90.0f);
                clockwise = false;
            } else {
                rotation = QQuaternion::fromAxisAndAngle(rotationLeftRightSideAxis, 90.0f);
                clockwise = true;
            }

            cubesOnSide = rubiksCube->getCubesOnSide('L');
            for (auto &cube : cubesOnSide) {
                cube->rotateCube(rotation);
            }
            rubiksCube->updateCubesAfterRotation('L', clockwise);
            break;
        case Qt::Key_D:
            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(rotationLeftRightSideAxis, 90.0f);
                clockwise = false;
            } else {
                rotation = QQuaternion::fromAxisAndAngle(rotationLeftRightSideAxis, -90.0f);
                clockwise = true;
            }

            cubesOnSide = rubiksCube->getCubesOnSide('R');
            for (auto &cube : cubesOnSide) {
                cube->rotateCube(rotation);
            }
            rubiksCube->updateCubesAfterRotation('R', clockwise);
            break;
        case Qt::Key_E:
            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(rotationFrontBackSideAxis, 90.0f);
                clockwise = false;
            } else {
                rotation = QQuaternion::fromAxisAndAngle(rotationFrontBackSideAxis, -90.0f);
                clockwise = true;
            }

            cubesOnSide = rubiksCube->getCubesOnSide('F');
            for (auto &cube : cubesOnSide) {
                cube->rotateCube(rotation);
            }
            rubiksCube->updateCubesAfterRotation('F', clockwise);
            break;
        case Qt::Key_Q:
            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(rotationFrontBackSideAxis, -90.0f);
                clockwise = false;
            } else {
                rotation = QQuaternion::fromAxisAndAngle(rotationFrontBackSideAxis, 90.0f);
                clockwise = true;
            }

            cubesOnSide = rubiksCube->getCubesOnSide('B');
            for (auto &cube : cubesOnSide) {
                cube->rotateCube(rotation);
            }
            rubiksCube->updateCubesAfterRotation('B', clockwise);
            break;
    }
}

