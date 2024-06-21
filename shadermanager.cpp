#include "shadermanager.h"

#include <QOpenGLFunctions>
#include <sstream>

ShaderManager::ShaderManager()
{}

ShaderManager::~ShaderManager()
{
    for (const auto shaderName : shadersOrder)
        delete shaders.at(shaderName);
}

void ShaderManager::initializeShader(ShaderID shaderId)
{
    shaders.at(shaderId)->initializeUniforms();
}

void ShaderManager::disableAttributeArray(ShaderID shaderId, const char* attribName)
{
    int attribLocation = getShader(shaderId)->attributeLocation(attribName);
    if (attribLocation == -1)
        qDebug() << attribName << "is not a valid attribute";
    getShader(shaderId)->disableAttributeArray(attribLocation);
}

void ShaderManager::setAttributeBuffer(ShaderID shaderId, const char *attribName,
                              GLenum type, int offset, int tupleSize, int stride)
{
    int attribLocation = getShader(shaderId)->attributeLocation(attribName);
    qDebug() << attribName << "is not a valid attribute";
    getShader(shaderId)->enableAttributeArray(attribLocation);
    getShader(shaderId)->setAttributeBuffer(attribLocation, type, offset,
                                            tupleSize, stride);
}

// Get a pointer to Shader object by ShaderID
Shader* ShaderManager::getShader(ShaderID shaderId)
{
    if (shaders.count(shaderId))
        return shaders.at(shaderId);
    else
    {
        qDebug() << "getShader: no such shader in shader map";
        return nullptr;
    }
}

ShaderID ShaderManager::getShaderOrderByIndex(int i) const
{
    return shadersOrder[i];
}

size_t ShaderManager::getIndexInOrder(GLuint shaderId) const
{
    for (size_t i = 0; i < shadersOrder.size(); i++)
    {
        if (shadersOrder[i] == shaderId)
            return i;
    }
    throw std::invalid_argument("ID not found in shaderOrder vector");
}

void ShaderManager::setShaderState(ShaderID shaderId, bool state)
{
    try
    {
        if (state)
            shaders.at(shaderId)->setActive();
        else
            shaders.at(shaderId)->setInactive();
    } catch (...)
    {
        qDebug() << "error thrown at setShaderState";
    }

}

bool ShaderManager::getShaderState(ShaderID shaderId) const
{
    return shaders.at(shaderId)->isActive();
}

void ShaderManager::moveShaderUp(ShaderID shaderId)
{
    for (int i = 2; i < shadersOrder.size(); i++)
    {
        if (shaderId == shadersOrder[i])
        {
            std::swap(shadersOrder[i], shadersOrder[i - 1]);
            break;
        }
    }
}

void ShaderManager::moveShaderDown(ShaderID shaderId)
{
    for (int i = 1; i < shadersOrder.size() - 1; i++)
    {
        if (shaderId == shadersOrder[i])
        {
            std::swap(shadersOrder[i], shadersOrder[i + 1]);
            break;
        }
    }
}

// Returns ptr to new shader and its index in shaderOrder
QPair<Shader*, int> ShaderManager::copyShader(GLuint shaderId)
{
    Shader* newShader = getShader(shaderId)->createCopy();
    newShader->compile();
    assert(newShader->getId() != shaderId);

    shaders.insert(std::make_pair(newShader->getId(), newShader));
    int indexToInsert = this->getIndexInOrder(shaderId) + 1;
    shadersOrder.insert(indexToInsert, newShader->getId());

    return qMakePair(newShader, indexToInsert);
}

// Returns index that deleted shader was at
// todo check if last
int ShaderManager::deleteShader(GLuint shaderId)
{
    delete shaders.at(shaderId);
    shaders[shaderId] = nullptr;
    shaders.erase(shaderId);

    int indexToRemove = getIndexInOrder(shaderId);
    shadersOrder.removeAt(indexToRemove);

    return indexToRemove;
}

const QVector<ShaderID>& ShaderManager::getCurrentOrder()
{
    return shadersOrder;
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

void ShaderManager::setInt(ShaderID shaderId, const char* name, const int value)
{
    shaders.at(shaderId)->setUniformValue(name, value);
}

void ShaderManager::setFloat(ShaderID shaderId, const char* name, const float value)
{
    shaders.at(shaderId)->setUniformValue(name, value);
}

void ShaderManager::setVec3(ShaderID shaderId, const char* name, const QVector3D& value)
{
    shaders.at(shaderId)->setUniformValue(name, value);
}

void ShaderManager::addShader(Shader* shader)
{
    shaders.insert(std::make_pair(shader->getId(), shader));
    shadersOrder.push_back(shader->getId());
}
