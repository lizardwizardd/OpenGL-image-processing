
#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <map>
#include <unordered_map>
#include <QOpenGLShaderProgram>

enum class ShaderName
{
    Base,
    Count
};

class ShaderManager
{
public:
    ShaderManager();
    ~ShaderManager();

    QOpenGLShaderProgram *getShader(ShaderName shaderName);

private:
    std::unordered_map<ShaderName, QOpenGLShaderProgram*> shaders;
    std::map<ShaderName, bool> shaderState;

    void initializeContainers();
    void addShader(ShaderName shaderName, const QString& vertexPath,
                                          const QString& fragmentPath);
};

#endif // SHADERMANAGER_H
