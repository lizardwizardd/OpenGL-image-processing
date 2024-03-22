
#include "shadermanager.h"

ShaderManager::ShaderManager()
{
    initializeContainers();
}

ShaderManager::~ShaderManager()
{
    for (auto& pair : shaderState)
        delete shaders.at(pair.first);
}

QOpenGLShaderProgram* ShaderManager::getShader(ShaderName shaderName)
{
    if (shaders.count(shaderName))
        return shaders.at(shaderName);
    else
        return nullptr;
}

void ShaderManager::setInt(ShaderName shader, char *name, int value)
{
    shaders.at(shader)->setUniformValue(name, value);
}

void ShaderManager::initializeContainers()
{
    addShader(ShaderName::Base,
              "F:/Programming/OpenGL-image-processing/shaders/base.vert",
              "F:/Programming/OpenGL-image-processing/shaders/base.frag");
    shaderState.at(ShaderName::Base) = true;
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
