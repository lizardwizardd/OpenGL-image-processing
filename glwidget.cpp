
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

    if (newTexture)
    {
        if (this->texture) // if replacing an existing texture
        {
            delete this->texture;
        }
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
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            qDebug() << "Framebuffer not complete!";
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to default framebuffer

        // Handle size change
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
}

void GLWidget::paintGL()
{
    // First Pass: Render to FBO using the first shader
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 0.0f, 1.0f, 0.5f);
    glUseProgram(shaderManager->getProgramId(ShaderName::Base));
    shaderManager->setInt(ShaderName::Base, (char*)"texture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glBindVertexArray(vao);

    glViewport(0, 0, viewportWidth, viewportHeight);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    // Second Pass: Render to screen using the second shader
    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to default framebuffer
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.0f, 1.0f, 1.0f, 0.5f);
    glUseProgram(shaderManager->getProgramId(ShaderName::Correction));
    shaderManager->setInt(ShaderName::Correction, (char*)"screenTexture", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glBindVertexArray(vao);

    glViewport(viewportBottomLeftX, viewportBottomLeftY, viewportWidth, viewportHeight);

    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GLWidget::resizeEvent(QResizeEvent *event)
{
    QOpenGLWindow::resizeEvent(event);

    if (!texture)
        return;

    // Get new window size
    int widthOfFramebuffer = event->size().width();
    int heightOfFramebuffer = event->size().height();

    // Calculate new viewport dimensions and position
    float requiredViewportHeight = widthOfFramebuffer * (1.0f / textureAspectRatio);

    if (requiredViewportHeight > heightOfFramebuffer)
    {
        float requiredViewportWidth = heightOfFramebuffer * textureAspectRatio;
        viewportWidth = (int)requiredViewportWidth;
        viewportHeight = heightOfFramebuffer;
        float verticalBarWidth = (widthOfFramebuffer - viewportWidth) / 2.0f;
        viewportBottomLeftX = (int)verticalBarWidth;
    }
    else
    {
        viewportWidth = widthOfFramebuffer;
        viewportHeight = (int)requiredViewportHeight;
        float horizontalBarHeight = (heightOfFramebuffer - viewportHeight) / 2.0f;
        viewportBottomLeftY = (int)horizontalBarHeight;
    }

    float scaleDiff = (float)viewportHeight / texture->height();
    qDebug() << "Height scale diff:" << scaleDiff;

    glUseProgram(shaderManager->getProgramId(ShaderName::Base));
    shaderManager->setFloat(ShaderName::Base, (char*)"scaleDiff", scaleDiff);

    glUseProgram(shaderManager->getProgramId(ShaderName::Correction));
    shaderManager->setFloat(ShaderName::Correction, (char*)"scaleDiff", scaleDiff);
}

void GLWidget::resizeGL(int w, int h)
{
    qDebug() << "resizegl is called";
}

void GLWidget::updateVertices(QVector<float>& newVertices)
{

}

void GLWidget::initializeBuffers()
{
    // Create FBO
    glGenFramebuffers(1, &fbo);
    //glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // Create VAO and VBO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    // Vertex data (positions and texture coordinates)
    float vertices[] = {
        // positions   // texture coords
        -1.0f,  1.0f,  0.0f, 1.0f, // top left
        -1.0f, -1.0f,  0.0f, 0.0f, // bottom left
         1.0f, -1.0f,  1.0f, 0.0f, // bottom right
         1.0f,  1.0f,  1.0f, 1.0f  // top right
    };
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // Set vertex attribute pointers
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Unbind VAO and VBO
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Load and create texture
    glGenTextures(1, &textureID);
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
