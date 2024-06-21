
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

    void addShader(Shader* shader);
    Shader *getShader(ShaderID shaderId);
    ShaderID getShaderOrderByIndex(int i) const;
    size_t getIndexInOrder(ShaderID shaderId) const;
    void setShaderState(ShaderID shaderId, bool state);
    int countActiveShaders();
    int getShaderCount();
    bool getShaderState(ShaderID shaderId) const;

    void moveShaderUp(ShaderID shaderId);
    void moveShaderDown(ShaderID shaderId);
    void copyShader(ShaderID shaderId);
    void deleteShader(ShaderID shaderId);

    const QVector<ShaderID>& getCurrentOrder();

private:
    std::unordered_map<ShaderID, Shader*> shaders;
    QVector<ShaderID> shadersOrder;
};

#endif // SHADERMANAGER_H
