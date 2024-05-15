#include "cubegeometry.h"

#include <QDebug>

CubeGeometry::CubeGeometry()
{}

CubeGeometry::CubeGeometry(float size, QVector<QVector3D> color, QVector3D position)
{
    initializeOpenGLFunctions();

    // Initializes cube geometry and transfers it to VBOs
    this->size = size;
    this->color = color;
    this->position = position;

    initCubeGeometry();
    initShader();

    prepareModel();
}

CubeGeometry::~CubeGeometry()
{
    vertBuff->destroy();
    indBuff->destroy();
}

void CubeGeometry::initShader()
{
    program = new QOpenGLShaderProgram();
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/Shaders/vertexShader.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/Shaders/fragmentShader.frag");
    program->link();
    if (program->isLinked())
    {
        qDebug() << "program is linked";
    }
    qDebug() << program->log();
}

void CubeGeometry::prepareModel()
{
    vao = new QOpenGLVertexArrayObject();
    vao->create();
    vao->bind();

    vertBuff = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    vertBuff->create();
    vertBuff->bind();
    vertBuff->setUsagePattern(QOpenGLBuffer::StaticDraw);
    vertBuff->allocate(&vertices[0], (int)(vertices.size() * sizeof(QVector3D)));

    int offset = 0;

    int vertexLocation = program->attributeLocation("a_pos");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, offset, 3, 2 * sizeof(QVector3D));

    offset = sizeof(QVector3D);

    int colorLocation = program->attributeLocation("a_color");
    program->enableAttributeArray(colorLocation);
    program->setAttributeBuffer(colorLocation, GL_FLOAT, offset, 3, 2 * sizeof(QVector3D));

    indBuff = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
    indBuff->create();
    indBuff->bind();
    indBuff->setUsagePattern(QOpenGLBuffer::StaticDraw);
    indBuff->allocate(&indices[0], (int)(indices.size() * sizeof(GLushort)));
    vao->release();
    qDebug() << "Model prepared";
}

void CubeGeometry::initCubeGeometry()
{
    vertices = {
        // positions          // colors
        // front side
        QVector3D(-size, -size, -size),  color[0],  // front bottom left 0
        QVector3D(size, -size, -size),   color[0],  // front bottom right 1
        QVector3D(size,  size, -size),  color[0],  // front top right 2
        QVector3D(-size,  size, -size),   color[0],  // front top left 3
        // top side
        QVector3D(size,  size, -size),   color[1],  // front top right 2
        QVector3D(-size,  size, -size),  color[1],  // front top left 3
        QVector3D(-size,  size,  size),   color[1],  // back top left 7
        QVector3D(size,  size,  size),  color[1],  // back top right 6
        // bottom side
        QVector3D(-size, -size, -size),   color[2],  // front bottom left 0
        QVector3D(size, -size, -size),  color[2],  // front bottom right 1
        QVector3D(size, -size,  size),   color[2],  // back bottom right 5
        QVector3D(-size, -size,  size),  color[2],  // back bottom left 4
        // right side
        QVector3D(size, -size, -size),  color[3],  // front bottom right 1
        QVector3D(size,  size, -size),  color[3],  // front top right 2
        QVector3D(size,  size,  size), color[3],  // back top right 6
        QVector3D(size, -size,  size),   color[3],  // back bottom right 5
        // left side
        QVector3D(-size, -size, -size),  color[4],  // front bottom left 0
        QVector3D(-size,  size, -size),  color[4],  // front top left 3
        QVector3D(-size,  size,  size), color[4],  // back top left 7
        QVector3D(-size, -size,  size),   color[4],  // back bottom left 4
        // back side
        QVector3D(-size, -size,  size),   color[5],  //   back bottom left 4
        QVector3D(size, -size,  size),    color[5],  // back bottom right 5
        QVector3D(size,  size,  size), color[5],  // back top right 6
        QVector3D(-size,  size,  size),  color[5]  // back top left 7
    };

    indices = {
        0, 1, 2, 2, 3, 0, // front
        4, 5, 6, 6, 7, 4, // top
        8, 9, 10, 10, 11, 8, // bottom
        12, 13, 14, 14, 15, 12, // right
        16, 17, 18, 18, 19, 16, // left
        20, 21, 22, 22, 23, 20 // back
    };
}

void CubeGeometry::drawCubeGeometry(QMatrix4x4 &projection, QMatrix4x4 &view)
{
    program->bind();
    program->setUniformValue("mvp_matrix", projection * view * model);

    vao->bind();

    glDrawElements(GL_TRIANGLES, (int)indices.size(), GL_UNSIGNED_SHORT, nullptr);

    vao->release();
    program->release();
}

void CubeGeometry::SetPosition(QVector3D position)
{
    this->position = position;
}

QVector3D CubeGeometry::GetPosition()
{
    return position;
}

void CubeGeometry::SetModel(QMatrix4x4 model)
{
    this->model = model;
}

QMatrix4x4 CubeGeometry::GetModel()
{
    return model;
}

void CubeGeometry::rotateCube(QQuaternion &rotation)
{
    this->targetRotation = rotation.normalized() * this->targetRotation;
}

void CubeGeometry::SetRotation(QQuaternion rotation)
{
    this->rotation = rotation;
}

QQuaternion CubeGeometry::GetRotation()
{
    return rotation;
}

void CubeGeometry::SetTargetRotation(QQuaternion targetRotation)
{
    this->targetRotation = targetRotation;
}

QQuaternion CubeGeometry::GetTargetRotation()
{
    return targetRotation;
}

// TODO: Rewrite this method
// void CubeGeometry::Rotate(QQuaternion rotation, int face)
// {
//     QMatrix4x4 rotMatrix = QMatrix4x4(rotation.toRotationMatrix());
//     QVector3D center = (vertices[face * 4] + vertices[face * 4 + 2]) / 2;
//     for (int i = 0; i < 4; i++)
//     {
//         vertices[face * 4 + i] = rotMatrix * (vertices[face * 4 + i] - center) + center;
//     }
//     vertBuff->bind();
//     vertBuff->write(0, &vertices[0], (int)(vertices.size() * sizeof(QVector3D)));
//     vertBuff->release();
// }
