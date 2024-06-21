
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
    void changeUniformValue(int sliderValue, ShaderID shaderId,
                            const char* uniformName);
    void changeUniformValue(const QVector3D color, ShaderID shaderId,
                            const char *uniformName);

    void handleShaderToggled(bool state, ShaderID shaderId);
    void handleShaderMoveUp(ShaderID shaderId);
    void handleShaderMoveDown(ShaderID shaderId);
    void handleShaderCopy(ShaderID shaderId);
    void handleShaderRemove(ShaderID shaderId);

    const std::vector<ShaderID>& getCurrentShaderOrder();
    const Shader* getShaderById(ShaderID shaderId);

signals:
    void imageSizeChanged(int width, int height);
    void glInitialized();

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
    void useShader(ShaderID shaderId);
    void createFramebuffers();
    void initializeShaders();
};

#endif // GLWIDGET_H
