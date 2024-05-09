
#include "shadermanager.h"
#include <QOpenGLFunctions>

ShaderManager::ShaderManager()
{}

ShaderManager::~ShaderManager()
{
    for (const auto shaderName : shadersOrder)
        delete shaders.at(shaderName);
}

GLuint ShaderManager::getProgramId(ShaderName shader)
{
    return getShader(shader)->programId();
}

void ShaderManager::initializeShader(ShaderName shaderName)
{
    shaders.at(shaderName)->initializeUniforms();
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

Shader* ShaderManager::getShader(ShaderName shaderName)
{
    if (shaders.count(shaderName))
        return shaders.at(shaderName);
    else
    {
        qDebug() << "getShader: no such shader in shader map";
        return nullptr;
    }
}

ShaderName ShaderManager::getShaderOrderByIndex(int i) const
{
    return shadersOrder[i];
}

void ShaderManager::setShaderState(ShaderName shaderName, bool state)
{
    if (state)
        shaders.at(shaderName)->setActive();
    else
        shaders.at(shaderName)->setInactive();
}

bool ShaderManager::getShaderState(ShaderName shaderName) const
{
    return shaders.at(shaderName)->isActive();
}

int ShaderManager::countActiveShaders()
{
    int count = 0;
    for (const auto name : shadersOrder)
    {
        if (shaders.at(name)->isActive())
            count++;
    }
    return count;
}

int ShaderManager::getShaderCount()
{
    return shadersOrder.size();
}

void ShaderManager::setInt(ShaderName shader, const char *name, const int value)
{
    shaders.at(shader)->setUniformValue(name, value);
}

void ShaderManager::setFloat(ShaderName shader, const char *name, const float value)
{
    shaders.at(shader)->setUniformValue(name, value);
}

void ShaderManager::addShader(Shader *shader)
{
    shaders.insert(std::make_pair(shader->getName(), shader));
    shadersOrder.push_back(shader->getName()); // delete later
}
