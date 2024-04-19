
#pragma once

#include <QOpenGLShaderProgram>

enum class ShaderName
{
    Base,
    Correction,
    Sharpness,
    Count
};

class Shader : public QOpenGLShaderProgram
{
public:
    Shader(const QString& vertexPath, const QString& fragmentPath, ShaderName shaderName) :
        vertexShaderPath(vertexPath),
        fragmentShaderPath(fragmentPath),
        name(shaderName)
    {}

    bool compile()
    {
        addShaderFromSourceFile(QOpenGLShader::Vertex, vertexShaderPath);
        addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentShaderPath);
        if (!link())
        {
            qCritical() << "Shader linking failed:" << log();
            return false;
        }
        return true;
    }

    ShaderName getName() const
    {
        return name;
    }

    virtual void initializeUniforms() = 0;

protected:
    QString vertexShaderPath;
    QString fragmentShaderPath;
    ShaderName name;
};


// BASE SHADER
class BaseShader : public Shader
{
public:
    BaseShader() : Shader(
            "F:/Programming/OpenGL-image-processing/shaders/base.vert",
            "F:/Programming/OpenGL-image-processing/shaders/base.frag",
            ShaderName::Base) {}

    void initializeUniforms() override {}
};


// COLOR CORRECTION SHADER
class CorrectionShader : public Shader
{
public:
    CorrectionShader() : Shader(
            "F:/Programming/OpenGL-image-processing/shaders/correction.vert",
            "F:/Programming/OpenGL-image-processing/shaders/correction.frag",
            ShaderName::Correction) {}

    void initializeUniforms() override
    {
        setUniformValue("exposure", exposureVals[2] / 100.0f);
        setUniformValue("contrast", contrastVals[2] / 100.0f);
        setUniformValue("temperature", temperatureVals[2] / 100.0f);
        setUniformValue("saturation", saturationVals[2] / 100.0f);
        setUniformValue("brightness", brightnessVals[2] / 100.0f);
    }
                                             //min   max  default
    static constexpr int exposureVals[3]    = {-200, 200, 0};
    static constexpr int contrastVals[3]    = {0,    200, 100};
    static constexpr int temperatureVals[3] = {0,    100, 0};
    static constexpr int saturationVals[3]  = {0,    200, 100};
    static constexpr int brightnessVals[3]  = {-100, 100, 0};
};


// SHARPNESS SHADER
class SharpnessShader : public Shader
{
public:
    SharpnessShader() : Shader(
            "F:/Programming/OpenGL-image-processing/shaders/sharpness.vert",
            "F:/Programming/OpenGL-image-processing/shaders/sharpness.frag",
            ShaderName::Sharpness) {}

    void initializeUniforms() override {}
};
