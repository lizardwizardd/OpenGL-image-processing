
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

    if (shaderManager)
    {
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

    bool needToDeallocate = false;
    bool textureUpdated = false;
    if (newTexture)
    {
        if (this->texture) // if replacing an existing texture
            needToDeallocate = true;
        textureUpdated = true;
    }
    else // load failed, go back
    {
        qDebug() << "Texture loading failed";
        delete newTexture;
        if (!texture) // if texture didn't exist (first load)
            this->QOpenGLWindow::hide();
    }

    if (needToDeallocate)
    {
        delete this->texture;
    }

    if (textureUpdated)
    {
        this->texture = newTexture;

        // Bind texture
        textureID = this->texture->textureId();
        glBindTexture(GL_TEXTURE_2D, textureID);

        // Initialize FBO texture
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glGenTextures(1, &textureColorbuffer);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, newTexture->width(),
              newTexture->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D,textureColorbuffer, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            qDebug() << "Framebuffer not complete!";
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to default framebuffer

        // Handle size change
        int windowW = texture->width();
        int windowH = texture->height();

        windowW = std::clamp(windowW, 150, 1820); // todo
        windowH = std::clamp(windowH, 150, 1000); //

        this->textureAspectRatio = (float)texture->width() / texture->height();
        this->QOpenGLWindow::resize(windowW, windowH);
        emit imageSizeChanged(windowW, windowH);
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
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // First Pass: Render to FBO using the first shader
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    //glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    glUseProgram(shaderManager->getProgramId(ShaderName::Base));
    shaderManager->setInt(ShaderName::Base, (char*)"texture", 0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glBindVertexArray(vaoStatic);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Second Pass: Render to screen using the second shader
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to default framebuffer
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.99f, 0.99f, 0.99f, 1.0f);

    glUseProgram(shaderManager->getProgramId(ShaderName::Correction));
    shaderManager->setInt(ShaderName::Correction, (char*)"screenTexture", 0);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GLWidget::resizeEvent(QResizeEvent *event)
{
    QOpenGLWindow::resizeEvent(event);

    if (!texture)
        return;

    float windowAspectRatio = (float)(event->size().width()) / event->size().height();
    float objectWidth, objectHeight;

    float scaleDiff;
    if (windowAspectRatio > textureAspectRatio)
    {
        // Width stretched, horizontal bars
        objectWidth = (float)textureAspectRatio / windowAspectRatio;
        objectHeight = 1.0f;
        scaleDiff = (float)event->size().height() / texture->height();
    }
    else
    {
        // Height stretched, vertical bars
        objectWidth = 1.0f;
        objectHeight = (float)windowAspectRatio / textureAspectRatio;
        scaleDiff = (float)event->size().height() * objectHeight / texture->height();
    }

    QVector<float> vertices1 = {
        -objectWidth,  objectHeight,    0.0f, 1.0f, // TL
        -objectWidth, -objectHeight,    0.0f, 0.0f, // BL
         objectWidth, -objectHeight,    1.0f, 0.0f, // BR
         objectWidth,  objectHeight,    1.0f, 1.0f  // TR
    };
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices1.size(),
                                                        vertices1.data());

    QVector<float> vertices2 = {
        -1.0f,                   -1.0f + objectHeight * 2,  0.0f, 1.0f, // TL
        -1.0f,                   -1.0f,                     0.0f, 0.0f, // BL
        -1.0f + objectWidth * 2, -1.0f,                     1.0f, 0.0f, // BR
        -1.0f + objectWidth * 2, -1.0f + objectHeight * 2,  1.0f, 1.0f  // TR
    };
    glBindBuffer(GL_ARRAY_BUFFER, vboStatic);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices2.size(),
                                                        vertices2.data());

    //qDebug() << "Height scale diff:" << scaleDiff;
    glUseProgram(shaderManager->getProgramId(ShaderName::Base));
    shaderManager->setFloat(ShaderName::Base, (char*)"scaleDiff", scaleDiff);
    glUseProgram(shaderManager->getProgramId(ShaderName::Correction));
    shaderManager->setFloat(ShaderName::Correction, (char*)"scaleDiff", scaleDiff);
}

void GLWidget::updateVertices(QVector<float>& newVertices)
{
    //glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * newVertices.size(), newVertices.data());
}

void GLWidget::initializeBuffers()
{
    float vertices[] = {
        // positions   // texture coords
        -1.0f,  1.0f,  0.0f, 1.0f, // top left
        -1.0f, -1.0f,  0.0f, 0.0f, // bottom left
         1.0f, -1.0f,  1.0f, 0.0f, // bottom right
         1.0f,  1.0f,  1.0f, 1.0f  // top right
    };

    // FBO
    glGenFramebuffers(1, &fbo);

    // VAO STATIC
    glGenVertexArrays(1, &vaoStatic);
    glBindVertexArray(vaoStatic);

    // VBO STATIC
    glGenBuffers(1, &vboStatic);
    glBindBuffer(GL_ARRAY_BUFFER, vboStatic);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)
                                                   (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // VBO
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)
                                                   (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO and VBOs
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Load and create texture
    glGenTextures(1, &textureID);

    glActiveTexture(GL_TEXTURE0);
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
