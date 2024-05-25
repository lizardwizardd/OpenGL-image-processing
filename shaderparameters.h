
#pragma once

#include <QOpenGLShaderProgram>
#include <QApplication>

enum class ShaderName
{
    Base,
    Correction,
    Sharpness,
    Posterize,
    Invert,
    Count
};

class Shader : public QOpenGLShaderProgram
{
protected:
    // min, max, default, uniform name, display name
    using ValueTuple = std::tuple<int, int, int, const char*, const char*>;

    QString vertexShaderPath;
    QString fragmentShaderPath;
    ShaderName name;
    bool state;

public:
    Shader(const QString& vertexPath, const QString& fragmentPath, ShaderName shaderName, bool activeState = false) :
        vertexShaderPath(vertexPath),
        fragmentShaderPath(fragmentPath),
        name(shaderName),
        state(activeState)
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

    virtual void initializeUniforms() = 0;

    ShaderName getName() const
        { return name; }

    bool isActive()
        { return state; }

    void setActive()
        { state = true; }

    void setInactive()
        { state = false; }
};


// BASE SHADER
class BaseShader : public Shader
{
public:
    BaseShader() : Shader(
            ":/shaders/base.vert",
            ":/shaders/base.frag",
            ShaderName::Base) {}

    void initializeUniforms() override {}
};


// COLOR CORRECTION SHADER
class CorrectionShader : public Shader
{
public:
    CorrectionShader() : Shader(
            ":/shaders/correction.vert",
            ":/shaders/correction.frag",
            ShaderName::Correction) {}

    static constexpr ValueTuple exposureVals    = {-200, 200, 0, "exposure", "Exposure"};
    static constexpr ValueTuple contrastVals    = {0,    200, 100, "contrast", "Contrast"};
    static constexpr ValueTuple temperatureVals = {-100, 100, 0, "temperature", "Temperature"};
    static constexpr ValueTuple saturationVals  = {0,    200, 100, "saturation", "Saturation"};
    static constexpr ValueTuple brightnessVals  = {-100, 100, 0, "brightness", "Brightness"};
    static constexpr ValueTuple tintColor       = {0,    0,   0, "tintColor", "Tint color"};
    static constexpr ValueTuple tintIntensity   = {0,    100, 0, "tintIntensity", "Tint intensity"};
    static constexpr ValueTuple filterColor     = {0,    0,   0, "filterColor", "Filter color"};
    static constexpr ValueTuple filterIntensity = {0,    100, 0, "filterIntensity", "Filter intensity"};

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

        setUniformValue(std::get<3>(tintColor), 1.0f, 1.0f, 1.0f);

        setUniformValue(std::get<3>(tintIntensity),
                        std::get<2>(tintIntensity) / 100.0f);

        setUniformValue(std::get<3>(filterColor), 1.0f, 1.0f, 1.0f);

        setUniformValue(std::get<3>(filterIntensity),
                        std::get<2>(filterIntensity) / 100.0f);
    }
};


// SHARPNESS SHADER
class SharpnessShader : public Shader
{
public:
    SharpnessShader() : Shader(
            ":/shaders/sharpness.vert",
            ":/shaders/sharpness.frag",
            ShaderName::Sharpness) {}

    static constexpr ValueTuple strengthVals = {0, 100, 10, "strength", "Strength"};

    void initializeUniforms() override
    {
        setUniformValue(std::get<3>(strengthVals),
                        std::get<2>(strengthVals) / 100.0f);
    }
};

// PIXELATION SHADER
class PosterizeShader : public Shader
{
public:
    PosterizeShader() : Shader(
            ":/shaders/posterize.vert",
            ":/shaders/posterize.frag",
            ShaderName::Posterize) {}

    static constexpr ValueTuple colorsVals = {2, 100, 30, "numColors", "Posterize levels"};
    static constexpr ValueTuple gammaVals = {1, 200, 100, "gamma", "Gamma"};

    void initializeUniforms() override
    {
        setUniformValue(std::get<3>(colorsVals),
                        std::get<2>(colorsVals) / 100.0f);
        setUniformValue(std::get<3>(gammaVals),
                        std::get<2>(gammaVals) / 100.0f);
    }
};

// COLOR REDUCTION SHADER
class InvertShader : public Shader
{
public:
    InvertShader() : Shader(
            ":/shaders/invert.vert",
            ":/shaders/invert.frag",
            ShaderName::Invert) {}

    void initializeUniforms() override
    {
    }
};
