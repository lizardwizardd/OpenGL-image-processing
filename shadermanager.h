
#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <map>
#include <unordered_map>
#include <QOpenGLShaderProgram>

enum class ShaderName
{
    Base,
    Correction,
    Count
};

class ShaderManager
{
public:
    ShaderManager();
    ~ShaderManager();

    GLuint getProgramId(ShaderName shader);
    void useProgram(ShaderName shader);
    void disableAttributeArray(ShaderName shader, const char* attribName);
    void setAttributeBuffer(ShaderName shader, const char* attribName,
                            GLenum type, int offset, int tupleSize, int stride);

    // Uniform setters
    void setInt(ShaderName shader, char* name, int value);
    void setFloat(ShaderName shader, char *name, float value);

    QOpenGLShaderProgram *getShader(ShaderName shaderName);

private:
    std::unordered_map<ShaderName, QOpenGLShaderProgram*> shaders;
    std::map<ShaderName, bool> shaderState;

    void initializeContainers();
    void addShader(ShaderName shaderName, const QString& vertexPath,
                                          const QString& fragmentPath);
};

#endif // SHADERMANAGER_H
