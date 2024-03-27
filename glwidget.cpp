
#include "glwidget.h"

#include <QOpenGLShaderProgram>
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

    if (newTexture)
    {
        if (this->texture) // if replacing an existing texture
        {
            this->texture->release();
            delete this->texture;
        }
        this->texture = newTexture;
        qDebug() << "Texture loaded successfully";

        texture->bind();
        shaderManager->setInt(ShaderName::Base, (char*)"texture", 0);

        this->resize(texture->width(), texture->height());
        this->textureAspectRatio = (float)texture->width() / texture->height();

        emit imageSizeChanged(texture->width(), texture->height());
    }
    else // load failed, go back
    {
        qDebug() << "Texture loading failed";
        delete newTexture;
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
    initializeObject();
    //glUseProgram(shaderManager->getShader(ShaderName::Base)->programId());
}

void GLWidget::paintGL()
{
    QElapsedTimer timer;
    timer.start();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderManager->getProgramId(ShaderName::Base));

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    qint64 elapsedMs = timer.elapsed();
    qDebug() << "paintGL:" << elapsedMs << "ms";
}

void GLWidget::resizeEvent(QResizeEvent *event)
{
    QOpenGLWindow::resizeEvent(event);

    float windowAspectRatio = (float)(event->size().width()) / event->size().height();
    //qDebug() << "Resizing. New AR: " << windowAspectRatio << "Texture AR: " << textureAspectRatio;
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

    QVector<GLfloat> vertices = {
        -objectWidth, -objectHeight,
         objectWidth, -objectHeight,
         objectWidth,  objectHeight,
        -objectWidth,  objectHeight
    };

    shaderManager->setFloat(ShaderName::Base, (char*)"objectAspectRatio", textureAspectRatio);
    shaderManager->setFloat(ShaderName::Base, (char*)"windowAspectRatio", windowAspectRatio);
    updateVertices(vertices);
}

void GLWidget::updateVertices(QVector<GLfloat>& newVertices)
{
    vertices = newVertices;
    vbo.write(0, vertices.constData(), vertices.size() * sizeof(GLfloat));
    update();
}

void GLWidget::initializeObject()
{
    //QOpenGLShaderProgram* currentShader = shaderManager->getShader(ShaderName::Base);
    //glUseProgram(currentShader->programId());
    //int vertexLocation = shaderManager->getShader(ShaderName::Base)->attributeLocation("vertex");
    //shaderManager->getShader(ShaderName::Base)->enableAttributeArray(vertexLocation);
    //shaderManager->getShader(ShaderName::Base)->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 2, 0);
    //int vertexLocation = currentShader->attributeLocation("vertex");
    //currentShader->enableAttributeArray(vertexLocation);
    //currentShader->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 2, 0);
}

void GLWidget::initializeBuffers()
{
    vbo.create();
    vbo.bind();
    vertices = {-1.0f,  1.0f,
                -1.0f, -1.0f,
                 1.0f, -1.0f,
                 1.0f,  1.0f};
    vbo.allocate(vertices.constData(), vertices.size() * sizeof(GLfloat));
    glUseProgram(shaderManager->getProgramId(ShaderName::Base));
    shaderManager->setAttributeBuffer(ShaderName::Base, "vertex", GL_FLOAT, 0, 2, 0);
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
