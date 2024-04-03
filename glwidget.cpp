
#include "glwidget.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QDebug>
#include <QElapsedTimer>


GLWidget::GLWidget(QMainWindow *parent) :
    QOpenGLWindow()
{
    this->parent = parent;
}

GLWidget::~GLWidget()
{
    qDebug() << "GLWidget destructor invoked";
    makeCurrent();

    vao.release();
    vao.destroy();

    vbo.release();
    vbo.destroy();

    if (shaderManager)
    {
        shaderManager->disableAttributeArray(ShaderName::Base, "vertex");
        delete shaderManager;
    }
    doneCurrent();
}

void GLWidget::loadTexture(const QString &filename)
{
    QElapsedTimer timer;
    timer.start();

    this->show(); // create context

    QOpenGLTexture* newTexture = new QOpenGLTexture(QImage(filename).mirrored());
    newTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    newTexture->setMagnificationFilter(QOpenGLTexture::Linear);

    QOpenGLTexture* newColorBuffer = new QOpenGLTexture(QImage(filename).mirrored());
    newColorBuffer->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    newColorBuffer->setMagnificationFilter(QOpenGLTexture::Linear);

    if (newTexture)
    {
        if (this->texture) // if replacing an existing texture
        {
            ///texture->release();
            delete this->texture;
            delete this->colorBuffer;
        }
        this->texture = newTexture;
        this->colorBuffer = newColorBuffer;

        glUseProgram(shaderManager->getProgramId(ShaderName::Correction));
        shaderManager->setInt(ShaderName::Base, (char*)"texture", 0);

        glUseProgram(shaderManager->getProgramId(ShaderName::Correction));
        shaderManager->setInt(ShaderName::Base, (char*)"screenTexture", 0);


        if (fbo)
            delete fbo;
        fbo = new QOpenGLFramebufferObject(
            colorBuffer->width(), colorBuffer->height(), GL_TEXTURE_2D);
        fbo->bind();
        GLuint colorBufId = colorBuffer->textureId();
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, colorBufId, 0);
        if (!(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE))
            qDebug() << "Framebuffer not complete";


        this->resize(texture->width(), texture->height());
        this->textureAspectRatio = (float)texture->width() / texture->height();

        emit imageSizeChanged(texture->width(), texture->height());
    }
    else // load failed, go back
    {
        qDebug() << "Texture loading failed";
        delete newTexture;
        delete newColorBuffer;
        if (!texture) // if texture didn't exist (first load)
            this->hide();
    }

    qint64 elapsedMs = timer.elapsed();
    qDebug() << "loadTexture:" << elapsedMs << "ms";
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    shaderManager = new ShaderManager();
    initializeBuffers();
}

void GLWidget::paintGL()
{
    fbo->bind(); // rendering to tmp fbo

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    texture->bind();

    glUseProgram(shaderManager->getProgramId(ShaderName::Base));
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // rendering to original fbo
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(shaderManager->getProgramId(ShaderName::Correction));
    colorBuffer->bind();
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GLWidget::resizeEvent(QResizeEvent *event)
{
    QOpenGLWindow::resizeEvent(event);

    float windowAspectRatio = (float)(event->size().width()) / event->size().height();
    float objectWidth, objectHeight;

    if (windowAspectRatio > textureAspectRatio)
    {
        objectWidth = (float)textureAspectRatio / windowAspectRatio;
        objectHeight = 1.0f;
    }
    else
    {
        objectWidth = 1.0f;
        objectHeight = (float)windowAspectRatio / textureAspectRatio;
    }

    QVector<float> vertices = {
       -objectWidth, -objectHeight,
        objectWidth, -objectHeight,
        objectWidth,  objectHeight,
       -objectWidth,  objectHeight
    };

    glUseProgram(shaderManager->getProgramId(ShaderName::Base));
    shaderManager->setFloat(ShaderName::Base, (char*)"objectAspectRatio", textureAspectRatio);
    shaderManager->setFloat(ShaderName::Base, (char*)"windowAspectRatio", windowAspectRatio);

    glUseProgram(shaderManager->getProgramId(ShaderName::Correction));
    shaderManager->setFloat(ShaderName::Correction, (char*)"objectAspectRatio", textureAspectRatio);
    shaderManager->setFloat(ShaderName::Correction, (char*)"windowAspectRatio", windowAspectRatio);

    updateVertices(vertices);
}

void GLWidget::updateVertices(QVector<float>& newVertices)
{
    vertices = newVertices;
    vbo.write(0, vertices.constData(), vertices.size() * sizeof(GLfloat));
    update();
}

void GLWidget::initializeBuffers()
{
    vao.create();
    vao.bind();

    vbo.create();
    vbo.bind();
    vertices = {
        -1.0f,  1.0f,
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f};

    vbo.allocate(vertices.constData(), vertices.size() * sizeof(GLfloat));
    glUseProgram(shaderManager->getProgramId(ShaderName::Base));
    shaderManager->setAttributeBuffer(ShaderName::Base, "vertex", GL_FLOAT, 0, 2, 0);
    glUseProgram(shaderManager->getProgramId(ShaderName::Correction));
    shaderManager->setAttributeBuffer(ShaderName::Correction, "vertex", GL_FLOAT, 0, 2, 0);
}


QSize GLWidget::minimumSizeHint() const
{
    return QSize(100, 100);
}

void GLWidget::closeEvent(QCloseEvent *event)
{
    if (parent)
        parent->close();
    event->accept();
}
