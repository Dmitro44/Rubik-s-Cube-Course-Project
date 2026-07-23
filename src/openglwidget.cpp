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
    // glClearColor(0.9529f, 0.9529f, 0.9529f, 1.0f);
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
        rubiksCube->changeRotationAxis(rotationUpDownSideAxis, 0);
        rubiksCube->changeRotationAxis(rotationFrontBackSideAxis, 1);

    } else if (rotationAroundAxis.y() != 0.0f) {
        tempRotationAxis = rotationFrontBackSideAxis;
        rotationFrontBackSideAxis = rotationLeftRightSideAxis;
        rotationLeftRightSideAxis = tempRotationAxis;

        if (!clockwise) {
            rotationFrontBackSideAxis = -rotationFrontBackSideAxis;
        } else {
            rotationLeftRightSideAxis = -rotationLeftRightSideAxis;
        }
        rubiksCube->changeRotationAxis(rotationFrontBackSideAxis, 1);
        rubiksCube->changeRotationAxis(rotationLeftRightSideAxis, 2);

    } else {
        tempRotationAxis = rotationUpDownSideAxis;
        rotationUpDownSideAxis = rotationLeftRightSideAxis;
        rotationLeftRightSideAxis = tempRotationAxis;

        if (!clockwise) {
            rotationLeftRightSideAxis = -rotationLeftRightSideAxis;
        } else {
            rotationUpDownSideAxis = -rotationUpDownSideAxis;
        }
        rubiksCube->changeRotationAxis(rotationUpDownSideAxis, 0);
        rubiksCube->changeRotationAxis(rotationLeftRightSideAxis, 2);
    }
}

void OpenGLWidget::updateScramble()
{
    rubiksCube->scramble();
}


void OpenGLWidget::keyPressEvent(QKeyEvent *event)
{
    // Perform rotation of the side of the cube based on the key pressed
    QQuaternion rotation;
    bool clockwise = true;
    QVector<CubeGeometry *> cubesOnSide;
    switch (event->key()) {
        case Qt::Key_W:
            if (!firstMoveFlag) {
                emit firstMove();
                firstMoveFlag = true;
            }

            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(rotationUpDownSideAxis, 90.0f);
                clockwise = false;
                rubiksCube->addToSolution("U' ");
            } else {
                rotation = QQuaternion::fromAxisAndAngle(rotationUpDownSideAxis, -90.0f);
                clockwise = true;
                rubiksCube->addToSolution("U ");
            }

            rubiksCube->rotateSide(rotation, 'U', clockwise);
            break;
        case Qt::Key_S:
            if (!firstMoveFlag) {
                emit firstMove();
                firstMoveFlag = true;
            }

            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(rotationUpDownSideAxis, -90.0f);
                clockwise = false;
                rubiksCube->addToSolution("D' ");
            } else {
                rotation = QQuaternion::fromAxisAndAngle(rotationUpDownSideAxis, 90.0f);
                clockwise = true;
                rubiksCube->addToSolution("D ");
            }

            rubiksCube->rotateSide(rotation, 'D', clockwise);
            break;
        case Qt::Key_A:
            if (!firstMoveFlag) {
                emit firstMove();
                firstMoveFlag = true;
            }

            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(rotationLeftRightSideAxis, -90.0f);
                clockwise = false;
                rubiksCube->addToSolution("L' ");
            } else {
                rotation = QQuaternion::fromAxisAndAngle(rotationLeftRightSideAxis, 90.0f);
                clockwise = true;
                rubiksCube->addToSolution("L ");
            }

            rubiksCube->rotateSide(rotation, 'L', clockwise);
            break;
        case Qt::Key_D:
            if (!firstMoveFlag) {
                emit firstMove();
                firstMoveFlag = true;
            }

            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(rotationLeftRightSideAxis, 90.0f);
                clockwise = false;
                rubiksCube->addToSolution("R' ");
            } else {
                rotation = QQuaternion::fromAxisAndAngle(rotationLeftRightSideAxis, -90.0f);
                clockwise = true;
                rubiksCube->addToSolution("R ");
            }

            rubiksCube->rotateSide(rotation, 'R', clockwise);
            break;
        case Qt::Key_E:
            if (!firstMoveFlag) {
                emit firstMove();
                firstMoveFlag = true;
            }

            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(rotationFrontBackSideAxis, 90.0f);
                clockwise = false;
                rubiksCube->addToSolution("F' ");
            } else {
                rotation = QQuaternion::fromAxisAndAngle(rotationFrontBackSideAxis, -90.0f);
                clockwise = true;
                rubiksCube->addToSolution("F ");
            }

            rubiksCube->rotateSide(rotation, 'F', clockwise);
            break;
        case Qt::Key_Q:
            if (!firstMoveFlag) {
                emit firstMove();
                firstMoveFlag = true;
            }

            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(rotationFrontBackSideAxis, -90.0f);
                clockwise = false;
                rubiksCube->addToSolution("B' ");
            } else {
                rotation = QQuaternion::fromAxisAndAngle(rotationFrontBackSideAxis, 90.0f);
                clockwise = true;
                rubiksCube->addToSolution("B ");
            }

            rubiksCube->rotateSide(rotation, 'B', clockwise);
            break;
    }
}

