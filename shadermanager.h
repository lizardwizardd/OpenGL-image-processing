
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
    void useProgram(ShaderName shader);
    void disableAttributeArray(ShaderName shader, const char* attribName);
    void setAttributeBuffer(ShaderName shader, const char* attribName,
                            GLenum type, int offset, int tupleSize, int stride);

    // Uniform setters
    void setInt(ShaderName shader, const char* name, const int value);
    void setFloat(ShaderName shader, const char *name, const float value);

    void addShader(Shader* shader);
    Shader *getShader(ShaderName shaderName);
    ShaderName getShaderOrderByIndex(int i) const;

    int countActiveShaders();

    int getShaderCount();
private:
    std::unordered_map<ShaderName, Shader*> shaders;
    std::vector<ShaderName> shadersOrder;

    void initializeContainers();
};

#endif // SHADERMANAGER_H
