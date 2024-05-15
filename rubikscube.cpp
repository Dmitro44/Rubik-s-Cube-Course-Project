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
}

void RubiksCube::rotateAllCubes(QVector3D rotationAxis, bool clockwise)
{
    vec3Cube tempCubes = cubes;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            for (int k = 0; k < 3; ++k) {
                if (rotationAxis.x() != 0.0f) {
                    if (clockwise) {
                        cubes[i][j][k] = tempCubes[i][2-k][j];
                    } else {
                        cubes[i][j][k] = tempCubes[i][k][2-j];
                    }
                } else if (rotationAxis.y() != 0.0f) {
                    if (clockwise) {
                        cubes[i][j][k] = tempCubes[k][j][2-i];
                    } else {
                        cubes[i][j][k] = tempCubes[2-k][j][i];
                    }
                } else {
                    if (clockwise) {
                        cubes[i][j][k] = tempCubes[2-j][i][k];
                    } else {
                        cubes[i][j][k] = tempCubes[j][2-i][k];
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
}
