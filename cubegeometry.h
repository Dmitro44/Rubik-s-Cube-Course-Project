#ifndef CUBEGEOMETRY_H
#define CUBEGEOMETRY_H

#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLBuffer>

class CubeGeometry : protected QOpenGLFunctions
{
public:
    CubeGeometry(float size, QVector<QVector3D> color, QVector3D position);
    virtual ~CubeGeometry();

    void initShader();
    void prepareModel();
    void drawCubeGeometry(QMatrix4x4 &projection, QMatrix4x4 &view, QMatrix4x4 &model);
    void SetPosition(QVector3D position);
    QVector3D GetPosition();

    bool intersects(const QVector3D& rayStart, const QVector3D& rayEnd);
    void Rotate(QQuaternion rotation, int face);

private:
    void initCubeGeometry(float size, QVector<QVector3D> color, QVector3D position);

    QOpenGLShaderProgram *program;
    QOpenGLBuffer *vertBuff;
    QOpenGLBuffer *indBuff;
    QOpenGLVertexArrayObject *vao;

    QVector3D position;
    QVector3D minBound;
    QVector3D maxBound;

    QVector<QVector3D> vertices;
    QVector<GLushort> indices;
};

#endif // CUBEGEOMETRY_H
