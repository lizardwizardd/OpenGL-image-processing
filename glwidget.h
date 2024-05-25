
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

    bool loadTexture(const QString &filename);
    void initializeUniforms();
    void changeUniformValue(int sliderValue, ShaderName shaderName,
                            const char* uniformName);

    void handleShaderToggled(bool state, ShaderName shaderName);
    void handleShaderMoveUp(ShaderName shader);
    void handleShaderMoveDown(ShaderName shader);
    void handleShaderCopy(ShaderName shader);
    void handleShaderRemove(ShaderName shader);

    void changeUniformValue(const QVector3D color, ShaderName shaderName, const char *uniformName);
signals:
    void imageSizeChanged(int width, int height);

protected:
    void initializeGL() override;
    void paintGL() override;
    void resizeEvent(QResizeEvent *event) override;

private:
    float scaleDiff;
    float textureAspectRatio = 0.0f;
    ShaderManager* shaderManager = nullptr;
    QMainWindow* parent = nullptr;
    
    GLuint vaoCentering;
    GLuint vboCentering;
    GLuint vaoNoCentering;
    GLuint vboNoCentering;

    GLuint textureID;
    QOpenGLTexture* texture = nullptr;

    std::vector<GLuint> fbos;
    std::vector<GLuint> colorBuffers;

    void initializeBuffers();
    void closeEvent(QCloseEvent *event) override;
    void useShader(ShaderName shaderName);
    void createFramebuffers();
};

#endif // GLWIDGET_H
