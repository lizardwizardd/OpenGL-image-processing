
#include "shadermanager.h"
#include <QOpenGLFunctions>

ShaderManager::ShaderManager()
{
    initializeContainers();
}

ShaderManager::~ShaderManager()
{
    for (auto& pair : shaderState)
        delete shaders.at(pair.first);
}

GLuint ShaderManager::getProgramId(ShaderName shader)
{
    return getShader(shader)->programId();
}

void ShaderManager::useProgram(ShaderName shader)
{
    ///glUseProgram(getShader(shader)->programId());
}

void ShaderManager::disableAttributeArray(ShaderName shader, const char* attribName)
{
    int attribLocation = getShader(shader)->attributeLocation(attribName);
    if (attribLocation == -1)
        qDebug() << attribName << "is not a valid attribute";
    getShader(shader)->disableAttributeArray(attribLocation);
}

void ShaderManager::setAttributeBuffer(ShaderName shader, const char *attribName,
                              GLenum type, int offset, int tupleSize, int stride)
{
    int attribLocation = getShader(shader)->attributeLocation(attribName);
    qDebug() << attribName << "is not a valid attribute";
    getShader(shader)->enableAttributeArray(attribLocation);
    getShader(shader)->setAttributeBuffer(attribLocation, type, offset,
                                          tupleSize, stride);
}

QOpenGLShaderProgram* ShaderManager::getShader(ShaderName shaderName)
{
    if (shaders.count(shaderName))
        return shaders.at(shaderName);
    else
    {
        qDebug() << "getShader: no such shader in shader map";
        return nullptr;
    }
}

void ShaderManager::setInt(ShaderName shader, char *name, int value)
{
    shaders.at(shader)->setUniformValue(name, value);
}

void ShaderManager::setFloat(ShaderName shader, char *name, float value)
{
    qDebug() << "Setting float " << value;
    shaders.at(shader)->setUniformValue(name, value);
}

void ShaderManager::initializeContainers()
{
    addShader(ShaderName::Base,
              "F:/Programming/OpenGL-image-processing/shaders/base.vert",
              "F:/Programming/OpenGL-image-processing/shaders/base.frag");
    shaderState.at(ShaderName::Base) = true;

    addShader(ShaderName::Correction,
              "F:/Programming/OpenGL-image-processing/shaders/correction.vert",
              "F:/Programming/OpenGL-image-processing/shaders/correction.frag");
    shaderState.at(ShaderName::Correction) = true;
}

void ShaderManager::addShader(ShaderName shaderName, const QString &vertexPath,
                                                   const QString &fragmentPath)
{
    QOpenGLShaderProgram* newShader = new QOpenGLShaderProgram();

    // TODO: Cacheable shaders
    newShader->addShaderFromSourceFile(QOpenGLShader::Vertex, vertexPath);
    newShader->addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentPath);

    if (newShader->link())
        qDebug() << "Linked successfully (addShader)";

    this->shaders.insert(std::make_pair(shaderName, newShader));
    this->shaderState.insert(std::make_pair(shaderName, false));
}
