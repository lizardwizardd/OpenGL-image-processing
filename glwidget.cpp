
#include "glwidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QDebug>

GLWidget::~GLWidget()
{
    makeCurrent();

    //int vertexLocation = shaderManager->getShader(ShaderName::Base)->attributeLocation("vertex");
    //shaderManager->getShader(ShaderName::Base)->disableAttributeArray(vertexLocation);

    vbo.destroy();
    delete shaderManager;
    doneCurrent();
}

void GLWidget::loadTexture(const QString &filename)
{
    auto oldTexture = this->texture;

    texture = new QOpenGLTexture(QImage(filename).mirrored());
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    if (texture)
    {
        qDebug() << "Texture loaded successfully";
        this->show();
        this->resize(texture->width(), texture->height());
        this->update();

        if (oldTexture)
            oldTexture->release(); // delete the old texture

        emit imageSizeChanged(texture->width(), texture->height());
    }
    else
    {
        qDebug() << "Texture loading failed";
        if (oldTexture)
            texture = oldTexture; // load the old texture
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
        shaderManager->setInt(ShaderName::Base, (char*)"texture", 0);
    }

    vbo.bind();
    // delete later
    int vertexLocation = shaderManager->getShader(ShaderName::Base)->attributeLocation("vertex");
    shaderManager->getShader(ShaderName::Base)->enableAttributeArray(vertexLocation);
    shaderManager->getShader(ShaderName::Base)->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 2, 0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // delete later
    shaderManager->getShader(ShaderName::Base)->disableAttributeArray(vertexLocation);
    vbo.release();
}

void GLWidget::makeObject()
{
    GLfloat vertices[] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };

    vbo.create();
    vbo.bind();
    vbo.allocate(vertices, sizeof(vertices));

    QOpenGLShaderProgram* currentShader = shaderManager->getShader(ShaderName::Base);
    glUseProgram(currentShader->programId());

    //int vertexLocation = currentShader->attributeLocation("vertex");
    //currentShader->enableAttributeArray(vertexLocation);
    //currentShader->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 2, 0);
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}
