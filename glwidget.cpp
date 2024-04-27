
#include "glwidget.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLTexture>
#include <QDebug>
#include <QElapsedTimer>
#include <QMessageBox>


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

    this->show();

    QOpenGLTexture* newTexture = new QOpenGLTexture(QImage(filename).mirrored());
    newTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    newTexture->setMagnificationFilter(QOpenGLTexture::Linear);

    bool needToDeallocate = false;
    bool textureUpdated = false;
    if (newTexture)
    {
        if (this->texture) // replacing an existing texture
            needToDeallocate = true;
        textureUpdated = true;
    }
    else // load failed, go back
    {
        qDebug() << "Texture loading failed";
        delete newTexture;
        if (!texture) // if texture didn't exist (first load)
            this->hide();
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

        createFramebuffers();

        // Handle size change
        int windowW = texture->width();
        int windowH = texture->height();

        ///windowW = std::clamp(windowW, 150, 1820);
        ///windowH = std::clamp(windowH, 150, 1000);
        this->textureAspectRatio = (float)texture->width() / texture->height();
        this->resize(windowW, windowH);
        this->setMinimumSize(QSize(texture->width(), texture->height()));
        ///emit imageSizeChanged(500, 500);
    }

    qint64 elapsedMs = timer.elapsed();
    qDebug() << "loadTexture:" << elapsedMs << "ms";

    this->update(); // TODO apply filter on first load
}

void GLWidget::changeUniformValue(int sliderValue, ShaderName shaderName,
                                  const char* uniformName)
{
    if (!shaderManager)
    {
        return; // allowing to change shader parameters before file was opened
    }
    useShader(shaderName);
    shaderManager->setFloat(shaderName, uniformName, (float)sliderValue / 100.0f);
    qDebug() << "setting" << (int)shaderName << uniformName << (float)sliderValue / 100.0f;
    this->update();
}

void GLWidget::initializeUniforms()
{
    useShader(ShaderName::Correction);
    shaderManager->initializeShader(ShaderName::Correction);
    useShader(ShaderName::Sharpness);
    shaderManager->initializeShader(ShaderName::Sharpness);
    useShader(ShaderName::Pixelate);
    shaderManager->initializeShader(ShaderName::Pixelate);
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    shaderManager = new ShaderManager();
    initializeBuffers();
    initializeUniforms();
}

void GLWidget::paintGL()
{
    int shadersCount = fbos.size() + 1;

    // N-1 passes rendering to FBOs
    for (int i = 0; i < shadersCount - 1; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);

        useShader(shaderManager->getShaderOrderByIndex(i));
        shaderManager->setInt(shaderManager->getShaderOrderByIndex(i),
                             (char*)"screenTexture", 0);

        glBindTexture(GL_TEXTURE_2D, (i == 0) ? textureID : colorBuffers[i - 1]);

        glBindVertexArray(vaoNoCentering);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    }

    // Last pass: Render to screen using the final shader
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.99f, 0.99f, 0.99f, 1.0f);

    useShader(shaderManager->getShaderOrderByIndex(shadersCount - 1));
    shaderManager->setInt(shaderManager->getShaderOrderByIndex(shadersCount - 1), (char*)"screenTexture", 0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[shadersCount - 2]);

    glBindVertexArray(vaoCentering);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GLWidget::createFramebuffers()
{
    int shadersCount = shaderManager->countActiveShaders();
    // Need N-1 FBOs and color buffers

    fbos.resize(shadersCount - 1);
    colorBuffers.resize(shadersCount - 1);

    glGenFramebuffers(shadersCount - 1, fbos.data());
    glGenTextures(shadersCount - 1, colorBuffers.data());

    for (int i = 0; i < shadersCount - 1; i++)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
        glBindTexture(GL_TEXTURE_2D, colorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width(),
                     texture->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, colorBuffers[i], 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            qDebug() << "Framebuffer not complete!";
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0); // Back to default framebuffer
}

void GLWidget::resizeEvent(QResizeEvent *event)
{
    if (!texture)
    {
        QOpenGLWindow::resizeEvent(event);
        return;
    }

    float scaleDiff;
    float objectWidth, objectHeight;
    float windowAspectRatio = (float)(event->size().width()) /
                                      event->size().height();
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

    useShader(ShaderName::Base);
    shaderManager->setFloat(ShaderName::Base, (char*)"scaleDiff", scaleDiff);
    useShader(ShaderName::Correction);
    shaderManager->setFloat(ShaderName::Correction, (char*)"scaleDiff", scaleDiff);
    useShader(ShaderName::Sharpness);
    shaderManager->setFloat(ShaderName::Sharpness, (char*)"scaleDiff", scaleDiff);
    shaderManager->setFloat(ShaderName::Sharpness, (char*)"textureWidth", texture->width());
    shaderManager->setFloat(ShaderName::Sharpness, (char*)"textureHeight", texture->height());
    useShader(ShaderName::Pixelate);
    shaderManager->setFloat(ShaderName::Pixelate, (char*)"scaleDiff", scaleDiff);

    QVector<float> vertices1 =
    {
        -objectWidth,  objectHeight,    0.0f, 1.0f, // TL
        -objectWidth, -objectHeight,    0.0f, 0.0f, // BL
         objectWidth, -objectHeight,    1.0f, 0.0f, // BR
         objectWidth,  objectHeight,    1.0f, 1.0f  // TR
    };

    QVector<float> vertices2 =
    {
        -1.0f,                   -1.0f + objectHeight * 2,  0.0f, 1.0f, // TL
        -1.0f,                   -1.0f,                     0.0f, 0.0f, // BL
        -1.0f + objectWidth * 2, -1.0f,                     1.0f, 0.0f, // BR
        -1.0f + objectWidth * 2, -1.0f + objectHeight * 2,  1.0f, 1.0f  // TR
    };

    // vertices 1
    glBindBuffer(GL_ARRAY_BUFFER, vboCentering);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices1.size(),
                                                        vertices1.data());
    // vertices 2
    glBindBuffer(GL_ARRAY_BUFFER, vboNoCentering);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * vertices2.size(),
                                                        vertices2.data());

    QOpenGLWindow::resizeEvent(event);
}

void GLWidget::updateVertices(QVector<float>& newVertices)
{}

void GLWidget::initializeBuffers()
{
    float vertices[] = {
        // positions   // texture coords
        -1.0f,  1.0f,  0.0f, 1.0f, // top left
        -1.0f, -1.0f,  0.0f, 0.0f, // bottom left
         1.0f, -1.0f,  1.0f, 0.0f, // bottom right
         1.0f,  1.0f,  1.0f, 1.0f  // top right
    };

    // VAO STATIC
    glGenVertexArrays(1, &vaoNoCentering);
    glBindVertexArray(vaoNoCentering);

    // VBO STATIC
    glGenBuffers(1, &vboNoCentering);
    glBindBuffer(GL_ARRAY_BUFFER, vboNoCentering);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)
                                                   (2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // VAO
    glGenVertexArrays(1, &vaoCentering);
    glBindVertexArray(vaoCentering);

    // VBO
    glGenBuffers(1, &vboCentering);
    glBindBuffer(GL_ARRAY_BUFFER, vboCentering);
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

void GLWidget::closeEvent(QCloseEvent *event)
{
    if (parent)
        parent->close();
    event->accept();
}

inline void GLWidget::useShader(ShaderName shaderName)
{
    glUseProgram(shaderManager->getProgramId(shaderName));
}
