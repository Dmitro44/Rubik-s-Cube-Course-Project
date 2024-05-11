#include "openglwidget.h"
#include <QtMath>

constexpr float interpolationFactor = 0.05f;

OpenGLWidget::OpenGLWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
    setFocusPolicy(Qt::StrongFocus);
    colors = {
        QVector3D(1.0f, 0.0f, 0.0f),
        QVector3D(0.0f, 1.0f, 0.0f),
        QVector3D(0.0f, 0.0f, 1.0f),
        QVector3D(1.0f, 1.0f, 0.0f),
        QVector3D(1.0f, 0.5f, 0.0f),
        QVector3D(1.0f, 1.0f, 1.0f)
    };

    // Camera setup
    cameraPos = QVector3D(3.0f, 3.0f, 3.0f);
    cameraFront = QVector3D(-0.525f, -0.525f, 0.525f) - cameraPos;
    cameraFront.normalize();
    cameraUp = QVector3D(0.0f, 1.0f, 0.0f);
}

void OpenGLWidget::initializeGL()
{
    makeCurrent();

    initializeOpenGLFunctions();
    setElementsOfCube(cubes, colors);

    glClearColor(0.7f, 1.0f, 0.7f, 1.0f);
}

void OpenGLWidget::resizeGL(int w, int h)
{
    makeCurrent();
    // Calculate aspect ratio
    qreal aspect = qreal(w) / qreal(h ? h : 1);

    // Set near plane to 3.0, far plane to 50.0, field of view 45 degrees
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

    // Calculate model view transformation

    view.setToIdentity();
    QVector3D front(0.0f, 0.0f, 0.0f);
    front.setX(cos(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch)));
    front.setY(sin(qDegreesToRadians(pitch)));
    front.setZ(sin(qDegreesToRadians(yaw)) * cos(qDegreesToRadians(pitch)));
    cameraFront = front.normalized();

    view.lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    // Draw cube geometry
    for (auto &cubeData : cubes)
    {
        model.setToIdentity();
        model.rotate(currentOrientation);
        model.translate(cubeData.cube.GetPosition());

        cubeData.cube.drawCubeGeometry(projection, view, model);
    }
    update();
}

void OpenGLWidget::setElementsOfCube(QVector<CubeData> &cubes, QVector<QVector3D> &colors)
{
    for (int x = 0; x < 3; ++x) {
        for (int y = 0; y < 3; ++y) {
            for (int z = 0; z < 3; ++z) {
                QVector<QVector3D> cubeColors;
                CubeFace cubeFace;
                for (int i = 0; i < 6; ++i)
                {
                    cubeColors.push_back(QVector3D(0.3f, 0.3f, 0.3f));
                }
                // Assign colors based on the position of the cube
                if (x == 0) { cubeColors[4] = colors[0]; cubeFace = CubeFace::Left; } // Red - left
                if (x == 2) { cubeColors[3] = colors[4]; cubeFace = CubeFace::Right; } // Orange - right
                if (y == 0) { cubeColors[2] = colors[3]; cubeFace = CubeFace::Bottom; } // Yellow - bottom
                if (y == 2) { cubeColors[1] = colors[5]; cubeFace = CubeFace::Top; } // White - top
                if (z == 0) { cubeColors[0] = colors[2]; cubeFace = CubeFace::Front; } // Blue - front
                if (z == 2) { cubeColors[5] = colors[1]; cubeFace = CubeFace::Back; } // Green - back
                QVector3D position(x * 0.525f - 0.525f, y * 0.525f - 0.525f, z * 0.525f - 0.525f);
                CubeGeometry cube(0.25f, cubeColors, position);
                cubes.push_back({cube, cubeFace});
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

    if (event->button() == Qt::LeftButton) {
        lastMousePos = event->pos();
        leftButtonPressed = true;
    }
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        rightButtonPressed = false;
    }

    if (event->button() == Qt::LeftButton) {
        leftButtonPressed = false;
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

    if (leftButtonPressed) {
        // TODO: swipe processing on the side of the Rubik's cube and turning this side in the direction of the swipe by 90 degrees

        float xoffset = lastMousePos.x() - event->x();
        float yoffset = lastMousePos.y() - event->y();

        lastMousePos = event->pos();

        if (abs(xoffset) > 1.0f || abs(yoffset) > 1.0f) {
            QVector3D rayStart = getRayWorld(lastMousePos);
            QVector3D rayEnd = getRayWorld(event->pos());

            // Find all cubes that was swiped over and add them to the selectedCubes vector
            QVector<CubeData*> selectedCubes;
            for (auto &cubeData : cubes) {
                if (cubeData.cube.intersects(rayStart, rayEnd)) {
                    selectedCubes.push_back(&cubeData);
                }
            }


            if (!selectedCubes.empty()) {
                // Calculate the total movement of the mouse
                float totalMovement = sqrt(xoffset * xoffset + yoffset * yoffset);

                // If the mouse has moved a certain distance, rotate the selected cubes by 90 degrees
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

                    for (auto &cubeData : selectedCubes) {
                        cubeData->cube.Rotate(rotation,  (int)cubeData->face);
                    }

                    leftButtonPressed = false;
                }

            }
        }
    }
}

QVector3D OpenGLWidget::getRayWorld(const QPoint &screenPos)
{
    float x = (2.0f * screenPos.x()) / width() - 1.0f;
    float y = 1.0f - (2.0f * screenPos.y()) / height();
    float z = 1.0f;
    QVector3D ray_nds(x, y, z);

    QVector4D ray_clip(ray_nds, 1.0f);

    QVector4D ray_eye = projection.inverted() * ray_clip;
    // ray_eye = QVector4D(ray_eye.x(), ray_eye.y(), -1.0f, 0.0f);

    QVector3D cameraForward_eye = (view * QVector4D(cameraFront, 0.0f)).toVector3D();
    ray_eye = QVector4D(ray_eye.x(), ray_eye.y(), -cameraForward_eye.z(), 0.0f);

    QVector3D ray_world = (view.inverted() * ray_eye).toVector3D();
    ray_world.normalize();

    return ray_world;
}
