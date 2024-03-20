
#include "glwidget.h"
#include <QOpenGLShaderProgram>
#include <QOpenGLTexture>
#include <QDebug>

GLWidget::~GLWidget()
{
    makeCurrent();
    vbo.destroy();
    delete program;
    doneCurrent();
}

void GLWidget::loadTexture(const QString &filename)
{
    texture = new QOpenGLTexture(QImage(filename).mirrored());
    texture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    texture->setMagnificationFilter(QOpenGLTexture::Linear);

    if (texture)
    {
        qDebug() << "Texture loaded successfully";
        this->update();
    }
}

void GLWidget::initializeGL()
{
    initializeOpenGLFunctions();
    makeObject();
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(program->programId());

    if (texture) {
        texture->bind();
        program->setUniformValue("texture", 0);
    }

    vbo.bind();
    int vertexLocation = program->attributeLocation("vertex");
    program->enableAttributeArray(vertexLocation);
    program->setAttributeBuffer(vertexLocation, GL_FLOAT, 0, 2, 0);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

    program->disableAttributeArray(vertexLocation);
    vbo.release();
    if (texture)
        texture->release();
}

void GLWidget::makeObject()
{
    GLfloat vertices[] = {
        -1.0f, -1.0f,
        1.0f, -1.0f,
        1.0f, 1.0f,
        -1.0f, 1.0f
    };

    vbo.create();
    vbo.bind();
    vbo.allocate(vertices, sizeof(vertices));

    program = new QOpenGLShaderProgram(this);
    program->addShaderFromSourceCode(QOpenGLShader::Vertex,
                                     "attribute highp vec4 vertex;\n"
                                     "varying highp vec2 texCoord;\n"
                                     "void main() {\n"
                                     "    gl_Position = vertex;\n"
                                     "    texCoord = vertex.xy * 0.5 + 0.5;\n"
                                     "}\n");
    program->addShaderFromSourceCode(QOpenGLShader::Fragment,
                                     "uniform sampler2D texture;\n"
                                     "varying highp vec2 texCoord;\n"
                                     "void main() {\n"
                                     "    gl_FragColor = texture2D(texture, texCoord);\n"
                                     "}\n");
    program->bindAttributeLocation("vertex", 0);
    program->link();
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize GLWidget::sizeHint() const
{
    return QSize(400, 400);
}
