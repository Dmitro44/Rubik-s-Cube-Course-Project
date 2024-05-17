#include "rubikscube.h"

RubiksCube::RubiksCube()
{
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

    rotationAxises.push_back(QVector3D(0.0f, 1.0f, 0.0f));
    rotationAxises.push_back(QVector3D(0.0f, 0.0f, 1.0f));
    rotationAxises.push_back(QVector3D(1.0f, 0.0f, 0.0f));
}

void RubiksCube::setElementsOfCube()
{
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
    solvedCubes = cubes;
}

void RubiksCube::rotateAllCubes(QVector3D rotationAxis, bool clockwise)
{
    vec3Cube tempCubes = cubes;
    vec3Cube tempSolvedCubes = solvedCubes;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                if (rotationAxis.x() != 0.0f) {
                    if (clockwise) {
                        cubes[i][j][k] = tempCubes[i][2-k][j];
                        solvedCubes[i][j][k] = tempSolvedCubes[i][2-k][j];
                    } else {
                        cubes[i][j][k] = tempCubes[i][k][2-j];
                        solvedCubes[i][j][k] = tempSolvedCubes[i][k][2-j];
                    }
                } else if (rotationAxis.y() != 0.0f) {
                    if (clockwise) {
                        cubes[i][j][k] = tempCubes[k][j][2-i];
                        solvedCubes[i][j][k] = tempSolvedCubes[k][j][2-i];
                    } else {
                        cubes[i][j][k] = tempCubes[2-k][j][i];
                        solvedCubes[i][j][k] = tempSolvedCubes[2-k][j][i];
                    }
                } else {
                    if (clockwise) {
                        cubes[i][j][k] = tempCubes[2-j][i][k];
                        solvedCubes[i][j][k] = tempSolvedCubes[2-j][i][k];
                    } else {
                        cubes[i][j][k] = tempCubes[j][2-i][k];
                        solvedCubes[i][j][k] = tempSolvedCubes[j][2-i][k];
                    }
                }
            }
        }
    }
}

QVector<CubeGeometry *> RubiksCube::getCubesOnSide(char side)
{
    QVector<CubeGeometry *> cubesOnSide;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            switch (side) {
            case 'U': cubesOnSide.push_back(&cubes[i][2][j]); break;
            case 'D': cubesOnSide.push_back(&cubes[i][0][j]); break;
            case 'L': cubesOnSide.push_back(&cubes[0][i][j]); break;
            case 'R': cubesOnSide.push_back(&cubes[2][i][j]); break;
            case 'F': cubesOnSide.push_back(&cubes[i][j][2]); break;
            case 'B': cubesOnSide.push_back(&cubes[i][j][0]); break;
            default: break;
            }
        }
    }
    return cubesOnSide;
}

void RubiksCube::updateCubesAfterRotation(char side, bool clockwise)
{
    switch (side) {
    case 'U': rotateFace(cubes, side, 2, clockwise, false, true); break;
    case 'D': rotateFace(cubes, side, 0, clockwise, false, true); break;
    case 'L': rotateFace(cubes, side, 0, clockwise, true, false); break;
    case 'R': rotateFace(cubes, side, 2, clockwise, true, false); break;
    case 'F': rotateFace(cubes, side, 2, clockwise, false, false); break;
    case 'B': rotateFace(cubes, side, 0, clockwise, false, false); break;
    }
}

void RubiksCube::rotateFace(vec3Cube &cubes, char side, int layer, bool clockwise, bool rotateX, bool rotateY)
{
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
                        cubes[i][j][layer] = tempCubes[2-j][i][layer];
                    }
                }
            }
        }
    }
    checkForSolved();
}

void RubiksCube::rotateSide(QQuaternion rotation, char side, bool clockwise)
{
    QVector<CubeGeometry*> cubesOnSide;
    cubesOnSide = getCubesOnSide(side);
    for (auto &cube : cubesOnSide) {
        cube->rotateCube(rotation);
    }
    updateCubesAfterRotation(side, clockwise);
}

void RubiksCube::scramble()
{
    QVector<char> moves = {'U', 'D', 'L', 'R', 'F', 'B'};
    QVector<char> lastThreeMoves(3, 0);
    int angle;
    for (int i = 0; i < 20; ++i) {
        int side;

        do {
            side = rand() % 6;
        } while (std::count(lastThreeMoves.begin(), lastThreeMoves.end(), moves[side]) > 1);
        std::rotate(lastThreeMoves.begin(), lastThreeMoves.begin() + 1, lastThreeMoves.end());
        lastThreeMoves[0] = moves[side];

        int clockwise = rand() % 2;
        switch (side) {
        case 0:
            if (clockwise) {
                angle = -90.0f;
                scrambleString += "U ";
            } else {
                angle = 90.0f;
                scrambleString += "U' ";
            }
            rotateSide(QQuaternion::fromAxisAndAngle(rotationAxises[0], angle), 'U', clockwise);
            break;
        case 1:
            if (clockwise) {
                angle = 90.0f;
                scrambleString += "D ";
            } else {
                angle = -90.0f;
                scrambleString += "D' ";
            }
            rotateSide(QQuaternion::fromAxisAndAngle(rotationAxises[0], angle), 'D', clockwise);
            break;
        case 2:
            if (clockwise) {
                angle = 90.0f;
                scrambleString += "L ";
            } else {
                angle = -90.0f;
                scrambleString += "L' ";
            }
            rotateSide(QQuaternion::fromAxisAndAngle(rotationAxises[2], angle), 'L', clockwise);
            break;
        case 3:
            if (clockwise) {
                angle = -90.0f;
                scrambleString += "R ";
            } else {
                angle = 90.0f;
                scrambleString += "R' ";
            }
            rotateSide(QQuaternion::fromAxisAndAngle(rotationAxises[2], angle), 'R', clockwise);
            break;
        case 4:
            if (clockwise) {
                angle = -90.0f;
                scrambleString += "F ";
            } else {
                angle = 90.0f;
                scrambleString += "F' ";
            }
            rotateSide(QQuaternion::fromAxisAndAngle(rotationAxises[1], angle), 'F', clockwise);
            break;
        case 5:
            if (clockwise) {
                angle = 90.0f;
                scrambleString += "B ";
            } else {
                angle = -90.0f;
                scrambleString += "B' ";
            }
            rotateSide(QQuaternion::fromAxisAndAngle(rotationAxises[1], angle), 'B', clockwise);
            break;
        }
    }
}

QString RubiksCube::getScramble()
{
    return scrambleString;
}

void RubiksCube::addToSolution(QString move)
{
    solutionString += move;
}

QString RubiksCube::getSolution()
{
    return solutionString;
}

void RubiksCube::checkForSolved()
{
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                if (cubes[i][j][k].GetPosition() != solvedCubes[i][j][k].GetPosition()) {
                    return;
                }
            }
        }
    }
    emit cubeSolved();
}

void RubiksCube::changeRotationAxis(QVector3D axis, int index)
{
    rotationAxises[index] = axis;
}
