
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWindow>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLFramebufferObject>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>
#include <QResizeEvent>
#include <QMainWindow>

#include "shadermanager.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GLWidget : public QOpenGLWindow, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QMainWindow* parent = nullptr);
    using QOpenGLWindow::QOpenGLWindow;
    ~GLWidget();

    void loadTexture(const QString &filename);

    QSize minimumSizeHint() const;

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

    QOpenGLTexture* texture = nullptr;
    QOpenGLTexture* colorBuffer = nullptr;
    QOpenGLFramebufferObject* fbo = nullptr;
    QOpenGLVertexArrayObject vao;
    QOpenGLBuffer vbo;
    //QOpenGLBuffer vboStatic;

    QVector<float> vertices;

    void initializeBuffers();
    void updateVertices(QVector<float>& newVertices);
    void closeEvent(QCloseEvent *event) override;

};

#endif // GLWIDGET_H
