#pragma once

#include <QOpenGLShaderProgram>
#include <QApplication>
#include <vector>

enum class ShaderName
{
    Base,
    Correction,
    Sharpness,
    Posterize,
    Invert,
    Count
};

enum class ParameterType
{
    SLIDER = 1,
    COLORPICKER = 2
};

class Shader : public QOpenGLShaderProgram
{
public:
    // min, max, default, uniform name, display name, parameter type
    using ValueTuple = std::tuple<int, int, int, const char*, const char*, ParameterType>;

protected:

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

    virtual void initializeUniforms()
    {
        for (const auto& param : getParameters())
        {
            int defaultValue = std::get<2>(param);
            const char* uniformName = std::get<3>(param);
            ParameterType paramType = std::get<5>(param);

            if (paramType == ParameterType::SLIDER)
            {
                setUniformValue(uniformName, defaultValue / 100.0f);
            }
            else if (paramType == ParameterType::COLORPICKER)
            {
                // Assuming default color is white (1.0, 1.0, 1.0)
                setUniformValue(uniformName, 1.0f, 1.0f, 1.0f);
            }
        }
    }

    GLuint getId() const
    { return this->programId(); }

    ShaderName getName() const
    { return name; }

    bool isActive() const
    { return state; }

    void setActive()
    { state = true; }

    void setInactive()
    { state = false; }

    virtual std::vector<ValueTuple> getParameters() const = 0;
    virtual const QString getTitle() const = 0;
    [[nodiscard]] virtual Shader* createCopy() const = 0;
};

// BASE SHADER
class BaseShader : public Shader
{
public:
    BaseShader() : Shader(
            ":/shaders/base.vert",
            ":/shaders/base.frag",
            ShaderName::Base) {}

    std::vector<ValueTuple> getParameters() const override
    {
        return {};
    }

    const QString getTitle() const override
    {
        return "Base";
    }

    [[nodiscard]] Shader* createCopy() const override
    {
        return new BaseShader();
    }
};


// COLOR CORRECTION SHADER
class CorrectionShader : public Shader
{
public:
    CorrectionShader() : Shader(
            ":/shaders/correction.vert",
            ":/shaders/correction.frag",
            ShaderName::Correction) {}

    std::vector<ValueTuple> getParameters() const override
    {
        return {
            {-200, 200, 0, "exposure", "Exposure", ParameterType::SLIDER},
            {0,    200, 100, "contrast", "Contrast", ParameterType::SLIDER},
            {-100, 100, 0, "temperature", "Temperature", ParameterType::SLIDER},
            {0,    200, 100, "saturation", "Saturation", ParameterType::SLIDER},
            {-100, 100, 0, "brightness", "Brightness", ParameterType::SLIDER},
            {0,    0,   0, "tintColor", "Tint color", ParameterType::COLORPICKER},
            {0,    100, 0, "tintIntensity", "Tint intensity", ParameterType::SLIDER},
            {0,    0,   0, "filterColor", "Filter color", ParameterType::COLORPICKER},
            {0,    100, 0, "filterIntensity", "Filter intensity", ParameterType::SLIDER}
        };
    }

    const QString getTitle() const override
    {
        return "Color Correction";
    }

    [[nodiscard]] Shader* createCopy() const override
    {
        return new CorrectionShader();
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

    std::vector<ValueTuple> getParameters() const override
    {
        return {
            {0, 100, 10, "strength", "Strength", ParameterType::SLIDER}
        };
    }

    const QString getTitle() const override
    {
        return "Sharpness";
    }

    [[nodiscard]] Shader* createCopy() const override
    {
        return new SharpnessShader();
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

    std::vector<ValueTuple> getParameters() const override
    {
        return {
            {2, 100, 30, "numColors", "Posterize levels", ParameterType::SLIDER},
            {1, 200, 100, "gamma", "Gamma", ParameterType::SLIDER}
        };
    }

    const QString getTitle() const override
    {
        return "Posterization";
    }

    [[nodiscard]] Shader* createCopy() const override
    {
        return new SharpnessShader();
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

    std::vector<ValueTuple> getParameters() const override
    {
        return {};
    }

    const QString getTitle() const override
    {
        return "Invert Colors";
    }

    [[nodiscard]] Shader* createCopy() const override
    {
        return new InvertShader();
    }
};
