
#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <map>
#include <unordered_map>
#include <QOpenGLShaderProgram>

#include "shaderparameters.h"

class ShaderManager
{
public:
    ShaderManager();
    ~ShaderManager();

    GLuint getProgramId(ShaderName shader);
    void initializeShader(ShaderName shaderName);
    void disableAttributeArray(ShaderName shader, const char* attribName);
    void setAttributeBuffer(ShaderName shader, const char* attribName,
                            GLenum type, int offset, int tupleSize, int stride);

    void setInt(ShaderName shader, const char* name, const int value);
    void setFloat(ShaderName shader, const char* name, const float value);
    void setVec3(ShaderName shader, const char* name, const QVector3D& value);

    void addShader(Shader* shader);
    Shader *getShader(ShaderName shaderName);
    ShaderName getShaderOrderByIndex(int i) const;
    void setShaderState(ShaderName shaderName, bool state);
    int countActiveShaders();
    int getShaderCount();
    bool getShaderState(ShaderName shaderName) const;

    void moveShaderUp(ShaderName shader);
    void moveShaderDown(ShaderName shader);

    const std::vector<ShaderName>& getCurrentOrder();

private:
    std::unordered_map<ShaderName, Shader*> shaders;
    std::vector<ShaderName> shadersOrder;
};

#endif // SHADERMANAGER_H
