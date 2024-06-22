#include "shaderparameters.h"

unsigned int BaseShader::copiesCreated = 0;
unsigned int CorrectionShader::copiesCreated = 0;
unsigned int SharpnessShader::copiesCreated = 0;
unsigned int PosterizeShader::copiesCreated = 0;
unsigned int InvertShader::copiesCreated = 0;


// BaseShader
BaseShader::BaseShader() : Shader(
        ":/shaders/base.vert",
        ":/shaders/base.frag",
        ShaderType::Base) {}

std::vector<Shader::ValueTuple> BaseShader::getParameters() const
{
    return {};
}

const QString BaseShader::getTitle() const
{
    return "Base";
}

const QString BaseShader::getTitleWithNumber() const
{
    if (copiesCreated > 0)
        return getTitle() + QString::number(copiesCreated);
    else
        return getTitle();
}

Shader* BaseShader::createCopy() const
{
    copiesCreated++;
    return new BaseShader();
}


// CorrectionShader
CorrectionShader::CorrectionShader() : Shader(
        ":/shaders/correction.vert",
        ":/shaders/correction.frag",
        ShaderType::Correction) {}

std::vector<Shader::ValueTuple> CorrectionShader::getParameters() const
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

const QString CorrectionShader::getTitle() const
{
    return "Color Correction";
}

const QString CorrectionShader::getTitleWithNumber() const
{
    if (copiesCreated > 0)
        return getTitle() + " " + QString::number(copiesCreated);
    else
        return getTitle();
}

Shader* CorrectionShader::createCopy() const
{
    copiesCreated++;
    return new CorrectionShader();
}


// SharpnessShader
SharpnessShader::SharpnessShader() : Shader(
        ":/shaders/sharpness.vert",
        ":/shaders/sharpness.frag",
        ShaderType::Sharpness) {}

std::vector<Shader::ValueTuple> SharpnessShader::getParameters() const
{
    return {
        {0, 100, 10, "strength", "Strength", ParameterType::SLIDER}
    };
}

const QString SharpnessShader::getTitle() const
{
    return "Sharpness";
}

const QString SharpnessShader::getTitleWithNumber() const
{
    if (copiesCreated > 0)
        return getTitle() + " " + QString::number(copiesCreated);
    else
        return getTitle();
}

Shader* SharpnessShader::createCopy() const
{
    copiesCreated++;
    return new SharpnessShader();
}


// PosterizeShader
PosterizeShader::PosterizeShader() : Shader(
        ":/shaders/posterize.vert",
        ":/shaders/posterize.frag",
        ShaderType::Posterize) {}

std::vector<Shader::ValueTuple> PosterizeShader::getParameters() const
{
    return {
        {2, 100, 30, "numColors", "Posterize levels", ParameterType::SLIDER},
        {1, 200, 100, "gamma", "Gamma", ParameterType::SLIDER}
    };
}

const QString PosterizeShader::getTitle() const
{
    return "Posterization";
}

const QString PosterizeShader::getTitleWithNumber() const
{
    if (copiesCreated > 0)
        return getTitle() + " " + QString::number(copiesCreated);
    else
        return getTitle();
}

Shader* PosterizeShader::createCopy() const
{
    copiesCreated++;
    return new PosterizeShader();
}


// InvertShader
InvertShader::InvertShader() : Shader(
        ":/shaders/invert.vert",
        ":/shaders/invert.frag",
        ShaderType::Invert) {}

std::vector<Shader::ValueTuple> InvertShader::getParameters() const
{
    return {};
}

const QString InvertShader::getTitle() const
{
    return "Invert Colors";
}

const QString InvertShader::getTitleWithNumber() const
{
    if (copiesCreated > 0)
        return getTitle() + " " + QString::number(copiesCreated);
    else
        return getTitle();
}

Shader* InvertShader::createCopy() const
{
    copiesCreated++;
    return new InvertShader();
}

