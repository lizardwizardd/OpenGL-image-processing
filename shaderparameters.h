
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

    virtual ~Shader()
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

    using ValueTuple = std::tuple<int, int, int, const char*>;
    static constexpr ValueTuple exposureVals    = {-200, 200, 0, "exposure"};
    static constexpr ValueTuple contrastVals    = {0, 200, 100, "contrast"};
    static constexpr ValueTuple temperatureVals = {0, 100, 0, "temperature"};
    static constexpr ValueTuple saturationVals  = {0, 200, 100, "saturation"};
    static constexpr ValueTuple brightnessVals  = {-100, 100, 0, "brightness"};

    void initializeUniforms() override
    {
        setUniformValue(std::get<3>(exposureVals),
                        std::get<2>(exposureVals) / 100.0f);

        setUniformValue(std::get<3>(contrastVals),
                        std::get<2>(contrastVals) / 100.0f);

        setUniformValue(std::get<3>(temperatureVals),
                        std::get<2>(temperatureVals) / 100.0f);

        setUniformValue(std::get<3>(saturationVals),
                        std::get<2>(saturationVals) / 100.0f);

        setUniformValue(std::get<3>(brightnessVals),
                        std::get<2>(brightnessVals) / 100.0f);
    }
};


// SHARPNESS SHADER
class SharpnessShader : public Shader
{
public:
    SharpnessShader() : Shader(
            "F:/Programming/OpenGL-image-processing/shaders/sharpness.vert",
            "F:/Programming/OpenGL-image-processing/shaders/sharpness.frag",
            ShaderName::Sharpness) {}

    using ValueTuple = std::tuple<int, int, int, const char*>;
    static constexpr ValueTuple strengthVals = {0, 100, 100, "strength"};

    void initializeUniforms() override
    {
        setUniformValue(std::get<3>(strengthVals),
                        std::get<2>(strengthVals) / 100.0f);
    }
};
