
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

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();

    // Initialize shaderManager and its containers
    shaderManager = new ShaderManager();

    initializeShaders();
    initializeBuffers();
    initializeUniforms();

    emit needToRecreateGUI();
}

void GLWidget::initializeShaders()
{
    Shader* currentShader;

    // Shaders are initialized inactive (except the base shader)
    currentShader = new BaseShader();
    currentShader->setActive();
    currentShader->compile();
    shaderManager->addShader(currentShader);

    currentShader = new CorrectionShader();
    currentShader->compile();
    shaderManager->addShader(currentShader);

    currentShader = new SharpnessShader();
    currentShader->compile();
    shaderManager->addShader(currentShader);

    currentShader = new PosterizeShader();
    currentShader->compile();
    shaderManager->addShader(currentShader);

    currentShader = new InvertShader();
    currentShader->compile();
    shaderManager->addShader(currentShader);
}

bool GLWidget::loadTexture(const QString &filename)
{
    QElapsedTimer timer;
    timer.start();

    this->show();

    bool textureLoaded = false;
    QImage originalImage(filename);
    QSize imageSize = originalImage.size();

    // Scale down if bigger than 1920x1000
    if (imageSize.width() > 1920 || imageSize.height() > 1000)
    {
        qreal scaleFactor = qMin(1920.0f / imageSize.width(), 1000.0f / imageSize.height());
        originalImage = originalImage.scaled(imageSize * scaleFactor,
                        Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QMessageBox messageBox;
        messageBox.setWindowTitle("Image is too big");
        messageBox.setText("Image is too big, it will be scaled down.");
        messageBox.setIcon(QMessageBox::Information);
        messageBox.exec();
    }

    QOpenGLTexture* newTexture = new QOpenGLTexture(originalImage.mirrored());
    newTexture->setMinificationFilter(QOpenGLTexture::NearestMipMapNearest);
    newTexture->setMagnificationFilter(QOpenGLTexture::Nearest);

    bool needToDeallocate = false;
    bool textureUpdated = false;
    if (newTexture)
    {
        if (this->texture) // replacing an existing texture
            needToDeallocate = true;
        textureUpdated = true;
        textureLoaded = true;
    }
    else // load failed, go back
    {
        qDebug() << "Texture loading failed";
        delete newTexture;
        if (texture) // if texture existed (not first load)
            textureLoaded = true;
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

        for (auto shaderId : getCurrentShaderOrder())
        {
            if (getShaderById(shaderId)->getName() == ShaderName::Sharpness)
            {
                useShader(shaderId);
                shaderManager->setFloat(shaderId, (char*)"textureWidth", texture->width());
                shaderManager->setFloat(shaderId, (char*)"textureHeight", texture->height());
            }
        }

        // Handle size change
        int windowW = newTexture->width();
        int windowH = newTexture->height();

        this->setMinimumSize(QSize(windowW, windowH));
        this->resize(windowW, windowH);
        this->textureAspectRatio = (float)windowW / windowH;
    }

    qint64 elapsedMs = timer.elapsed();
    qDebug() << "loadTexture:" << elapsedMs << "ms";
    this->update();

    return textureLoaded;
}

void GLWidget::paintGL()
{
    int shadersCount = shaderManager->getShaderCount();
    int activeShadersCount = shaderManager->countActiveShaders();

    // UNIFORMS

    // Set scaleDiff to all shaders except the last one
    ShaderID lastActiveShader = getCurrentShaderOrder()[0]; // first, always active
    for (int i = 0; i < shadersCount; i++)
    {
        ShaderID currentShader = shaderManager->getShaderOrderByIndex(i);
        if (shaderManager->getShaderState(currentShader))
            lastActiveShader = currentShader;

        useShader(currentShader);
        shaderManager->setFloat(currentShader, (char*)"scaleDiff", scaleDiff);
    }
    useShader(lastActiveShader);
    shaderManager->setFloat(lastActiveShader, (char*)"scaleDiff", 1.0f);

    // RENDER

    if (activeShadersCount == 1)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.99f, 0.99f, 0.99f, 1.0f);

        useShader(shaderManager->getShaderOrderByIndex(0));
        shaderManager->setInt(shaderManager->getShaderOrderByIndex(0),
                              (char*)"screenTexture", 0);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glBindVertexArray(vaoCentering);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        return;
    }

    // N-1 passes rendering to FBOs
    int timesRendered = 0;
    int i = 0;
    int lastFboIndex = 0;
    for (; i < shadersCount; i++)
    {
        // Dont use framebuffer for last active shader
        if (timesRendered + 1 >= activeShadersCount)
            break;

        if (fbos[i] == 0)
            continue; // skip inactive

        timesRendered++;

        glBindFramebuffer(GL_FRAMEBUFFER, fbos[i]);
        useShader(shaderManager->getShaderOrderByIndex(i));
        shaderManager->setInt(shaderManager->getShaderOrderByIndex(i),
                              (char*)"screenTexture", 0);
        glBindTexture(GL_TEXTURE_2D, (i == 0) ? textureID : colorBuffers[lastFboIndex]);
        glBindVertexArray(vaoNoCentering);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

        lastFboIndex = i;
    }

    // Skip to the last active shader
    while (fbos[i] == 0)
    {
        i++;
    }

    // Render to screen using the last active shader
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    glClearColor(0.99f, 0.99f, 0.99f, 1.0f);

    useShader(shaderManager->getShaderOrderByIndex(i));
    shaderManager->setInt(shaderManager->getShaderOrderByIndex(i),
                          (char*)"screenTexture", 0);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[lastFboIndex]);

    glBindVertexArray(vaoCentering);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void GLWidget::resizeEvent(QResizeEvent *event)
{
    if (!texture)
    {
        QOpenGLWindow::resizeEvent(event);
        return;
    }

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

    // Set scaleDiff to all shaders except the last one
    int shadersCount = fbos.size() + 1;
    ShaderID lastActiveShader = getCurrentShaderOrder()[0]; // first, always active
    for (int i = 0; i < shadersCount; i++)
    {
        ShaderID currentShader = shaderManager->getShaderOrderByIndex(i);
        if (shaderManager->getShaderState(currentShader))
            lastActiveShader = currentShader;

        useShader(currentShader);
        shaderManager->setFloat(currentShader, (char*)"scaleDiff", scaleDiff);
    }
    useShader(lastActiveShader);
    shaderManager->setFloat(lastActiveShader, (char*)"scaleDiff", 1.0f);

    // Update vertices
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

void GLWidget::initializeBuffers()
{
    float vertices[] = {
        // positions   // texture coords
        -1.0f,  1.0f,  0.0f, 1.0f, // top left
        -1.0f, -1.0f,  0.0f, 0.0f, // bottom left
         1.0f, -1.0f,  1.0f, 0.0f, // bottom right
         1.0f,  1.0f,  1.0f, 1.0f  // top right
    };

    // VAO
    glGenVertexArrays(1, &vaoNoCentering);
    glBindVertexArray(vaoNoCentering);

    // VBO
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

// Initialize framebuffers based on the number of active shaders
void GLWidget::createFramebuffers()
{
    int shadersCount = shaderManager->getShaderCount();
    int activeShadersCount = shaderManager->countActiveShaders();

    fbos.resize(shadersCount - 1, 0);
    colorBuffers.resize(shadersCount - 1, 0);

    glGenFramebuffers(shadersCount - 1, fbos.data());
    glGenTextures(shadersCount - 1, colorBuffers.data());

    // Create a framebuffer for each active shader except the last one
    int buffersCreated = 0;
    for (int i = 0; i < shadersCount - 1; i++)
    {
        // Dont create framebuffer if shader at i is inactive
        if (!shaderManager->getShader(shaderManager->
                                      getShaderOrderByIndex(i))->isActive())
        {
            fbos[i] = 0;
            colorBuffers[i] = 0;
            continue;
        }

        // Dont create framebuffer for the last active shader
        if (buffersCreated + 1 >= activeShadersCount)
        {
            break;
        }

        buffersCreated++;

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

void GLWidget::changeUniformValue(int sliderValue, ShaderID shaderId,
                                  const char* uniformName)
{
    if (!shaderManager)
    {
        return; // allowing to change shader parameters before file was opened
    }
    useShader(shaderId);
    shaderManager->setFloat(shaderId, uniformName, (float)sliderValue / 100.0f);
    //qDebug() << "setting" << (int)shaderName << uniformName << (float)sliderValue / 100.0f;
    this->update();
}

void GLWidget::changeUniformValue(const QVector3D color, ShaderID shaderId,
                                  const char* uniformName)
{
    if (!shaderManager)
    {
        return; // allowing to change shader parameters before file was opened
    }
    useShader(shaderId);
    shaderManager->setVec3(shaderId, uniformName, color);
    this->update();
}

void GLWidget::handleShaderToggled(bool state, ShaderID shaderId)
{
    if (!shaderManager)
        return;
    shaderManager->setShaderState(shaderId, state);

    glDeleteFramebuffers(fbos.size(), fbos.data());
    glDeleteTextures(colorBuffers.size(), colorBuffers.data());

    createFramebuffers();
    update();
}

void GLWidget::handleShaderMoveUp(ShaderID shaderId)
{
    shaderManager->moveShaderUp(shaderId);
    createFramebuffers();
    this->update();
}

void GLWidget::handleShaderMoveDown(ShaderID shaderId)
{
    shaderManager->moveShaderDown(shaderId);
    createFramebuffers();
    this->update();
}

void GLWidget::handleShaderCopy(ShaderID shaderId)
{
    shaderManager->copyShader(shaderId);
    emit needToRecreateGUI();
    this->update();
}

void GLWidget::handleShaderRemove(ShaderID shaderId)
{
    shaderManager->deleteShader(shaderId);
    emit needToRecreateGUI();
    this->update();
}

const QVector<ShaderID> &GLWidget::getCurrentShaderOrder()
{
    return shaderManager->getCurrentOrder();
}

const Shader *GLWidget::getShaderById(ShaderID shaderId)
{
    return shaderManager->getShader(shaderId);
}

void GLWidget::initializeUniforms()
{
    for (int i = 0; i < shaderManager->getShaderCount(); i++)
    {
        ShaderID currentId = shaderManager->getShaderOrderByIndex(i);
        useShader(currentId);
        shaderManager->initializeShader(currentId);
    }
}

void GLWidget::closeEvent(QCloseEvent *event)
{
    if (parent)
        parent->close();
    event->accept();
}

inline void GLWidget::useShader(ShaderID shaderId)
{
    glUseProgram(shaderId);
}
