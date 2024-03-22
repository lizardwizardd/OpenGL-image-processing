
#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLBuffer>

#include "shadermanager.h"

QT_FORWARD_DECLARE_CLASS(QOpenGLShaderProgram);
QT_FORWARD_DECLARE_CLASS(QOpenGLTexture)

class GLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    using QOpenGLWidget::QOpenGLWidget;
    ~GLWidget();

    void loadTexture(const QString &filename);

    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

signals:
    void imageSizeChanged(int width, int height);

protected:
    void initializeGL() override;
    void paintGL() override;

private:
    void makeObject();
    QOpenGLTexture* texture = nullptr;
    ShaderManager* shaderManager;
    QOpenGLBuffer vbo;
};

#endif // GLWIDGET_H
