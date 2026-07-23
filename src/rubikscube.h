#ifndef RUBIKSCUBE_H
#define RUBIKSCUBE_H

#include <QObject>

#include "cubegeometry.h"

#define vec3Cube QVector<QVector<QVector<CubeGeometry>>> // 3x3x3 cube

class RubiksCube : public QObject
{
    Q_OBJECT
public:
    RubiksCube();

    void setElementsOfCube();

    void rotateAllCubes(QVector3D rotationAxis, bool clockwise);

    QVector<CubeGeometry *> getCubesOnSide(char side);

    void updateCubesAfterRotation(char side, bool clockwise);

    void rotateFace(vec3Cube &cubes, char side, int layer, bool clockwise, bool rotateX, bool rotateY);

    void rotateSide(QQuaternion rotation, char side, bool clockwise);

    void scramble();

    QString getScramble();

    void addToSolution(QString move);

    QString getSolution();

    void checkForSolved();

    vec3Cube &getCubes() { return cubes; }

    void changeRotationAxis(QVector3D axis, int index);

signals:
    void cubeSolved();

private:
    vec3Cube cubes;
    QVector<QVector3D> colors;

    QVector<QVector3D> rotationAxises;

    QString scrambleString;
    QString solutionString;

    vec3Cube solvedCubes;
};

#endif // RUBIKSCUBE_H
