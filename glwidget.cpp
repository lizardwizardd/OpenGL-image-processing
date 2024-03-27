
#include "glwidget.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QDebug>


GLWidget::GLWidget(QMainWindow *parent) :
    QOpenGLWindow()
{
    this->parent = parent;
}

GLWidget::~GLWidget()
{
    qDebug() << "GLWidget destructor invoked";
    makeCurrent();
    //int vertexLocation = shaderManager->getShader(ShaderName::Base)->attributeLocation("vertex");
    //shaderManager->getShader(ShaderName::Base)->disableAttributeArray(vertexLocation);
    int vertexLocation = shaderManager->getShader(ShaderName::Base)->attributeLocation("vertex");
    shaderManager->getShader(ShaderName::Base)->disableAttributeArray(vertexLocation);
    vbo.release();
    vbo.destroy();
    delete shaderManager;
    doneCurrent();
}

void GLWidget::loadTexture(const QString &filename)
{
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderManager->getShader(ShaderName::Base)->programId());

    // TODO: move this part to loadTexture

    //vbo.bind();
    // delete later
    //int vertexLocation = shaderManager->getShader(ShaderName::Base)->attributeLocation("vertex");
    //shaderManager->getShader(ShaderName::Base)->enableAttributeArray(vertexLocation);
    //shaderManager->getShader(ShaderName::Base)->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 2, 0);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // delete later
    //shaderManager->getShader(ShaderName::Base)->disableAttributeArray(vertexLocation);
    //vbo.release();
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
    vertices = newVertices; //

    //vbo.bind();
    vbo.write(0, vertices.constData(), vertices.size() * sizeof(GLfloat));
    //vbo.release();

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
    // new
    glUseProgram(shaderManager->getShader(ShaderName::Base)->programId());
    int vertexLocation = shaderManager->getShader(ShaderName::Base)->attributeLocation("vertex");
    shaderManager->getShader(ShaderName::Base)->enableAttributeArray(vertexLocation);
    shaderManager->getShader(ShaderName::Base)->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 2, 0);
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
