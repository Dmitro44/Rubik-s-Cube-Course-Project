#include "openglwidget.h"
#include <QtMath>

constexpr float interpolationFactor = 0.05f;

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
        setFocusPolicy(Qt::StrongFocus);
    colors = {
        QVector3D(1.0f, 0.0f, 0.0f), // red
        QVector3D(0.0f, 1.0f, 0.0f), // green
        QVector3D(0.0f, 0.0f, 1.0f), // blue
        QVector3D(1.0f, 1.0f, 0.0f), // yellow
        QVector3D(1.0f, 0.5f, 0.0f), // orange
        QVector3D(1.0f, 1.0f, 1.0f)  // white
    };
    cubes.resize(3);
    for (auto &cube2D : cubes) {
        cube2D.resize(3);
        for (auto &cube1D : cube2D) {
            cube1D.resize(3);
        }
    }

    setupCamera();
}

void OpenGLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    setElementsOfCube(cubes, colors);
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
    // cameraPos = QVector3D(0.0f, 5.0f, 0.0f);
    // cameraFront = QVector3D(0.0f, 0.0f, 1.0f);
    cameraFront = QVector3D(-0.525f, -0.525f, 0.525f) - cameraPos;
    cameraFront.normalize();
    cameraUp = QVector3D(0.0f, 1.0f, 0.0f);
}

void OpenGLWidget::updateCubesAfterRotation(vec3Cube &cubes, char side, bool clockwise) {
    switch (side) {
    case 'U': rotateFace(cubes, side, 2, clockwise, false, true); break;
    case 'D': rotateFace(cubes, side, 0, clockwise, false, true); break;
    case 'L': rotateFace(cubes, side, 0, clockwise, true, false); break;
    case 'R': rotateFace(cubes, side, 2, clockwise, true, false); break;
    case 'F': rotateFace(cubes, side, 0, clockwise, false, false); break;
    case 'B': rotateFace(cubes, side, 2, clockwise, false, false); break;
    }
}

void OpenGLWidget::rotateFace(vec3Cube &cubes, char side, int layer, bool clockwise, bool rotateX, bool rotateY) {
    vec3Cube tempCubes = cubes;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if (rotateX) { // Rotate around X-axis (L/R)
                if (clockwise) {
                    if (side == 'R') {
                        cubes[layer][j][i] = tempCubes[layer][2-i][j];
                    } else {
                        cubes[layer][j][i] = tempCubes[layer][i][2-j];
                    }
                } else {
                    if (side == 'R') {
                        cubes[layer][j][i] = tempCubes[layer][i][2-j];
                    } else {
                        cubes[layer][j][i] = tempCubes[layer][2-i][j];
                    }
                }
            } else if (rotateY) { // Rotate around Y-axis (U/D)
                if (clockwise) {
                    if (side == 'U') {
                        cubes[i][layer][j] = tempCubes[j][layer][2-i];
                    } else {
                        cubes[i][layer][j] = tempCubes[2-j][layer][i];
                    }
                } else {
                    if (side == 'U') {
                        cubes[i][layer][j] = tempCubes[2-j][layer][i];
                    } else {
                        cubes[i][layer][j] = tempCubes[j][layer][2-i];
                    }
                }
            } else { // Rotate around Z-axis (F/B)
                if (clockwise) {
                    if (side == 'F') {
                        cubes[i][j][layer] = tempCubes[2-j][i][layer];
                    } else {
                        cubes[i][j][layer] = tempCubes[j][2-i][layer];
                    }
                } else {
                    if (side == 'F') {
                        cubes[i][j][layer] = tempCubes[j][2-i][layer];
                    } else {
                        cubes[i][j][layer] = tempCubes[j][2-i][layer];
                    }
                }
            }
        }
    }
}

QVector<CubeGeometry *> OpenGLWidget::getCubesOnSide(char side)
{
    QVector<CubeGeometry *> cubesOnSide;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            switch (side) {
            case 'U': cubesOnSide.push_back(&cubes[i][2][j]); break;
            case 'D': cubesOnSide.push_back(&cubes[i][0][j]); break;
            case 'L': cubesOnSide.push_back(&cubes[0][i][j]); break;
            case 'R': cubesOnSide.push_back(&cubes[2][i][j]); break;
            case 'F': cubesOnSide.push_back(&cubes[i][j][0]); break;
            case 'B': cubesOnSide.push_back(&cubes[i][j][2]); break;
            default: break;
            }
        }
    }
    return cubesOnSide;
}

void OpenGLWidget::setElementsOfCube(vec3Cube &cubes, QVector<QVector3D> &colors)
{
    // QVector3D position(-0.525f, -0.525f, -0.525f);
    // CubeGeometry cube(0.25f, colors, position);
    // cubes[0][0][0] = cube;

    // position = QVector3D(-0.525f, -0.525f, 0.0f);
    // CubeGeometry cube2(0.25f, colors, position);
    // cubes[0][0][1] = cube2;

    // position = QVector3D(-0.525f, -0.525f, 0.525f);
    // CubeGeometry cube3(0.25f, colors, position);
    // cubes[0][0][2] = cube3;

    // position = QVector3D(-0.525f, 0.0f, -0.525f);
    // CubeGeometry cube4(0.25f, colors, position);
    // cubes[0][1][0] = cube4;

    // position = QVector3D(-0.525f, 0.0f, 0.0f);
    // CubeGeometry cube5(0.25f, colors, position);
    // cubes[0][1][1] = cube5;
    // cubes[0][1][2] = cube5;

    for (int x = 0; x < 3; ++x) {
        for (int y = 0; y < 3; ++y) {
            for (int z = 0; z < 3; ++z) {
                QVector<QVector3D> cubeColors;
                for (int i = 0; i < 6; ++i)
                {
                    cubeColors.push_back(QVector3D(0.3f, 0.3f, 0.3f));
                }
                // Assign colors based on the position of the cube
                if (x == 0) cubeColors[4] = colors[2]; // Blue - left
                if (x == 2) cubeColors[3] = colors[1]; // Green - right
                if (y == 0) cubeColors[2] = colors[3]; // Yellow - bottom
                if (y == 2) cubeColors[1] = colors[5]; // White - top
                if (z == 0) cubeColors[0] = colors[0]; // Red - back
                if (z == 2) cubeColors[5] = colors[4]; // Orange - front
                QVector3D position(x * 0.525f - 0.525f, y * 0.525f - 0.525f, z * 0.525f - 0.525f);
                CubeGeometry cube(0.25f, cubeColors, position);
                cubes[x][y][z] = cube;
            }
        }
    }
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
            if (abs(xoffset) > abs(yoffset)) {
                rotationAxis = QVector3D(0.0f, 1.0f, 0.0f);
                angle = xoffset > 0 ? -90.0f : 90.0f;
            } else {
                if (event->x() < width() / 2)
                {
                    rotationAxis = QVector3D(1.0f, 0.0f, 0.0f);
                } else {
                    rotationAxis = QVector3D(0.0f, 0.0f, 1.0f);
                    yoffset = -yoffset;
                }
                angle = yoffset > 0 ? -90.0f : 90.0f;
            }

            QQuaternion rotation = QQuaternion::fromAxisAndAngle(rotationAxis, angle);

            targetOrientation = rotation.normalized() * targetOrientation;


            rightButtonPressed = false;
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
                rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 1.0f, 0.0f), 90.0f);
                clockwise = false;
            } else {
                rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 1.0f, 0.0f), -90.0f);
                clockwise = true;
            }

            cubesOnSide = getCubesOnSide('U');
            for (auto &cube : cubesOnSide) {
                cube->rotateCube(rotation);
            }
            updateCubesAfterRotation(cubes, 'U', clockwise);
            break;
        case Qt::Key_S:
            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 1.0f, 0.0f), -90.0f);
                clockwise = false;
            } else {
                rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 1.0f, 0.0f), 90.0f);
                clockwise = true;
            }

            cubesOnSide = getCubesOnSide('D');
            for (auto &cube : cubesOnSide) {
                cube->rotateCube(rotation);
            }
            updateCubesAfterRotation(cubes, 'D', clockwise);
            break;
        case Qt::Key_A:
            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), -90.0f);
                clockwise = false;
            } else {
                rotation = QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), 90.0f);
                clockwise = true;
            }

            cubesOnSide = getCubesOnSide('L');
            for (auto &cube : cubesOnSide) {
                cube->rotateCube(rotation);
            }
            updateCubesAfterRotation(cubes, 'L', clockwise);
            break;
        case Qt::Key_D:
            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), 90.0f);
                clockwise = false;
            } else {
                rotation = QQuaternion::fromAxisAndAngle(QVector3D(1.0f, 0.0f, 0.0f), -90.0f);
                clockwise = true;
            }

            cubesOnSide = getCubesOnSide('R');
            for (auto &cube : cubesOnSide) {
                cube->rotateCube(rotation);
            }
            updateCubesAfterRotation(cubes, 'R', clockwise);
            break;
        case Qt::Key_E:
            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 0.0f, 1.0f), 90.0f);
                clockwise = false;
            } else {
                rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 0.0f, 1.0f), -90.0f);
                clockwise = true;
            }

            cubesOnSide = getCubesOnSide('F');
            for (auto &cube : cubesOnSide) {
                cube->rotateCube(rotation);
            }
            updateCubesAfterRotation(cubes, 'F', clockwise);
            break;
        case Qt::Key_Q:
            if (event->modifiers() & Qt::ShiftModifier) {
                rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 0.0f, 1.0f), -90.0f);
                clockwise = false;
            } else {
                rotation = QQuaternion::fromAxisAndAngle(QVector3D(0.0f, 0.0f, 1.0f), 90.0f);
                clockwise = true;
            }

            cubesOnSide = getCubesOnSide('B');
            for (auto &cube : cubesOnSide) {
                cube->rotateCube(rotation);
            }
            updateCubesAfterRotation(cubes, 'B', clockwise);
            break;
    }
}

