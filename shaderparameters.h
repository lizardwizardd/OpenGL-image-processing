#pragma once

#include <QOpenGLShaderProgram>
#include <QApplication>
#include <vector>

enum class ShaderType
{
    Base,
    Correction,
    Sharpness,
    Posterize,
    Invert,
    Pixelate,
    Crt,
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
    ShaderType name;
    bool state;

public:
    Shader(const QString& vertexPath, const QString& fragmentPath,
           ShaderType shaderName, bool activeState = false) :
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
                setUniformValue(uniformName, 1.0f, 1.0f, 1.0f);
            }
        }
    }

    GLuint getId() const
    { return this->programId(); }

    ShaderType getName() const
    { return name; }

    bool isActive() const
    { return state; }

    void setActive()
    { state = true; }

    void setInactive()
    { state = false; }

    virtual std::vector<ValueTuple> getParameters() const = 0;
    virtual const QString getTitle() const = 0;
    virtual const QString getTitleWithNumber() const = 0;
    [[nodiscard]] virtual Shader* createCopy() const = 0;
};

// BASE SHADER
class BaseShader : public Shader
{
private:
    static unsigned int copiesCreated;

public:
    BaseShader();

    std::vector<ValueTuple> getParameters() const override;
    const QString getTitle() const override;
    const QString getTitleWithNumber() const override;
    [[nodiscard]] Shader* createCopy() const override;
};


// COLOR CORRECTION SHADER
class CorrectionShader : public Shader
{
private:
    static unsigned int copiesCreated;

public:
    CorrectionShader();

    std::vector<ValueTuple> getParameters() const override;
    const QString getTitle() const override;
    const QString getTitleWithNumber() const override;
    [[nodiscard]] Shader* createCopy() const override;
};


// SHARPNESS SHADER
class SharpnessShader : public Shader
{
private:
    static unsigned int copiesCreated;

public:
    SharpnessShader();

    std::vector<ValueTuple> getParameters() const override;
    const QString getTitle() const override;
    const QString getTitleWithNumber() const override;
    [[nodiscard]] Shader* createCopy() const override;
};


// POSTERIZATION SHADER
class PosterizeShader : public Shader
{
private:
    static unsigned int copiesCreated;

public:
    PosterizeShader();

    std::vector<ValueTuple> getParameters() const override;
    const QString getTitle() const override;
    const QString getTitleWithNumber() const override;
    [[nodiscard]] Shader* createCopy() const override;
};


// COLOR REDUCTION SHADER
class InvertShader : public Shader
{
private:
    static unsigned int copiesCreated;

public:
    InvertShader();

    std::vector<ValueTuple> getParameters() const override;
    const QString getTitle() const override;
    const QString getTitleWithNumber() const override;
    [[nodiscard]] Shader* createCopy() const override;
};


// PIXELIZATION SHADER
class PixelateShader : public Shader
{
private:
    static unsigned int copiesCreated;

public:
    PixelateShader();

    std::vector<ValueTuple> getParameters() const override;
    const QString getTitle() const override;
    const QString getTitleWithNumber() const override;
    [[nodiscard]] Shader* createCopy() const override;
};


// CRT SHADER
class CrtShader : public Shader
{
private:
    static unsigned int copiesCreated;

public:
    CrtShader();

    std::vector<ValueTuple> getParameters() const override;
    const QString getTitle() const override;
    const QString getTitleWithNumber() const override;
    [[nodiscard]] Shader* createCopy() const override;
};
