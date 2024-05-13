#ifndef CUBEGEOMETRY_H
#define CUBEGEOMETRY_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class CubeGeometry : protected QOpenGLFunctions
{
public:
    CubeGeometry();
    CubeGeometry(float size, QVector<QVector3D> color, QVector3D position);
    virtual ~CubeGeometry();

    void initShader();
    void prepareModel();
    void drawCubeGeometry(QMatrix4x4 &projection, QMatrix4x4 &view);
    void SetPosition(QVector3D position);
    QVector3D GetPosition();

    void SetModel(QMatrix4x4 model);
    QMatrix4x4 GetModel();

    void rotateCube(QQuaternion &rotation);

    void SetRotation(QQuaternion rotation);
    QQuaternion GetRotation();

    void SetTargetRotation(QQuaternion targetRotation);
    QQuaternion GetTargetRotation();


private:
    void initCubeGeometry();

    QOpenGLShaderProgram *program;
    QOpenGLBuffer *vertBuff;
    QOpenGLBuffer *indBuff;
    QOpenGLVertexArrayObject *vao;

    float size;
    QVector<QVector3D> color;

    QVector3D position;
    QMatrix4x4 model;

    QQuaternion rotation;
    QQuaternion targetRotation;

    QVector<QVector3D> vertices;
    QVector<GLushort> indices;
};

#endif // CUBEGEOMETRY_H
