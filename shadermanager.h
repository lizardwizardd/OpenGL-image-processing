
#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <map>
#include <unordered_map>
#include <QVector>
#include <QOpenGLShaderProgram>

#include "shaderparameters.h"

#define ShaderID GLuint

class ShaderManager
{
public:
    ShaderManager();
    ~ShaderManager();

    void initializeShader(ShaderID shaderId);
    void disableAttributeArray(ShaderID shaderId, const char* attribName);
    void setAttributeBuffer(ShaderID shaderId, const char* attribName,
                            GLenum type, int offset, int tupleSize, int stride);

    void setInt(ShaderID shaderId, const char* name, const int value);
    void setFloat(ShaderID shaderId, const char* name, const float value);
    void setVec3(ShaderID shaderId, const char* name, const QVector3D& value);
    void setVec2(ShaderID shaderId, const char* name, const QVector2D& value);

    void addShader(Shader* shader);
    void addShader(Shader* shader, int insertIndex);
    void moveShaderUp(ShaderID shaderId);
    void moveShaderDown(ShaderID shaderId);
    QPair<Shader*, int> copyShader(ShaderID shaderId);
    int deleteShader(ShaderID shaderId);

    Shader *getShader(ShaderID shaderId);
    ShaderID getShaderOrderByIndex(int i) const;
    size_t getIndexInOrder(ShaderID shaderId) const;
    void setShaderState(ShaderID shaderId, bool state);
    int countActiveShaders();
    int getShaderCount();
    bool getShaderState(ShaderID shaderId) const;
    unsigned int getTypeCopiesCount(const ShaderType shaderType);

    const QVector<ShaderID>& getCurrentOrder();

private:
    std::unordered_map<ShaderID, Shader*> shaders;
    std::unordered_map<ShaderType, unsigned int> typeCopiesCount;
    QVector<ShaderID> shadersOrder;
};

#endif // SHADERMANAGER_H
