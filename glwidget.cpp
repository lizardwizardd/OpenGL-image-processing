
#include "glwidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QDebug>

GLWidget::~GLWidget()
{
    makeCurrent();
    vbo.destroy();
    delete shaderManager;
    doneCurrent();
}

void GLWidget::loadTexture(const QString &filename)
{
    texture = new QOpenGLTexture(QImage(filename).mirrored());
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    if (texture)
    {
        qDebug() << "Texture loaded successfully";
        this->update();
    }
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    shaderManager = new ShaderManager();
    makeObject();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderManager->getShader(ShaderName::Base)->programId());

    if (texture) {
        texture->bind();
        shaderManager->getShader(ShaderName::Base)->setUniformValue("texture", 0);
    }

    vbo.bind();
    int vertexLocation = shaderManager->getShader(ShaderName::Base)->attributeLocation("vertex");
    shaderManager->getShader(ShaderName::Base)->enableAttributeArray(vertexLocation);
    shaderManager->getShader(ShaderName::Base)->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 2, 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    shaderManager->getShader(ShaderName::Base)->disableAttributeArray(vertexLocation);
    vbo.release();
    if (texture)
        texture->release();
}

void GLWidget::makeObject()
{
    GLfloat vertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f
    };

    vbo.create();
    vbo.bind();
    vbo.allocate(vertices, sizeof(vertices));

    QOpenGLShaderProgram* currentShader = shaderManager->getShader(ShaderName::Base);
    glUseProgram(currentShader->programId());
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}
