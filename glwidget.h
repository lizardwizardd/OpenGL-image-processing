
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWindow>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLBuffer>
#include <QResizeEvent>
#include <QMainWindow>

#include "shadermanager.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GLWidget : public QOpenGLWindow, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    GLWidget(QMainWindow* parent = nullptr);
    using QOpenGLWindow::QOpenGLWindow;
    ~GLWidget();

    void loadTexture(const QString &filename);

    void changeUniformValue(int sliderValue, ShaderName shaderName,
                            const char* uniformName);
    void initializeUniforms();

signals:
    void imageSizeChanged(int width, int height);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeEvent(QResizeEvent *event) override;

private:
    float textureAspectRatio = 0.0f;
    ShaderManager* shaderManager = nullptr;
    QMainWindow* parent = nullptr;

    GLuint vaoCorrection;
    GLuint vboCorrection;
    GLuint vaoBase;
    GLuint vboBase;

    GLuint textureID;
    QOpenGLTexture* texture = nullptr;

    GLuint fbo;
    GLuint fbo2;
    GLuint textureColorbuffer;
    GLuint textureColorbuffer2;

    void initializeBuffers();
    void updateVertices(QVector<float>& newVertices);
    void closeEvent(QCloseEvent *event) override;
    void useShader(ShaderName shaderName);
};

#endif // GLWIDGET_H
