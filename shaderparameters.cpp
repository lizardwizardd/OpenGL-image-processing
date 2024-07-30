#include "shaderparameters.h"

// BaseShader
unsigned int BaseShader::copiesCreated = 0;

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
unsigned int CorrectionShader::copiesCreated = 0;

CorrectionShader::CorrectionShader() : Shader(
        ":/shaders/default.vert",
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
unsigned int SharpnessShader::copiesCreated = 0;

SharpnessShader::SharpnessShader() : Shader(
        ":/shaders/default.vert",
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
unsigned int PosterizeShader::copiesCreated = 0;

PosterizeShader::PosterizeShader() : Shader(
        ":/shaders/default.vert",
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
unsigned int InvertShader::copiesCreated = 0;

InvertShader::InvertShader() : Shader(
        ":/shaders/default.vert",
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


// PixelateShader
unsigned int PixelateShader::copiesCreated = 0;

PixelateShader::PixelateShader() : Shader(
        ":/shaders/default.vert",
        ":/shaders/pixelate.frag",
        ShaderType::Pixelate) {}

std::vector<Shader::ValueTuple> PixelateShader::getParameters() const
{
    return {
        {1, 64, 4, "pixelSize", "Pixel size", ParameterType::SLIDER}
    };
}

const QString PixelateShader::getTitle() const
{
    return "Pixelate";
}

const QString PixelateShader::getTitleWithNumber() const
{
    if (this->copiesCreated > 0)
        return getTitle() + " " + QString::number(this->copiesCreated);
    else
        return getTitle();
}

Shader* PixelateShader::createCopy() const
{
    this->copiesCreated++;
    return new PixelateShader();
}


// CrtShader
unsigned int CrtShader::copiesCreated = 0;

CrtShader::CrtShader() : Shader(
        ":/shaders/default.vert",
        ":/shaders/crt.frag",
        ShaderType::Pixelate) {}

std::vector<Shader::ValueTuple> CrtShader::getParameters() const
{
    return {};
}

const QString CrtShader::getTitle() const
{
    return "CRT Effect";
}

const QString CrtShader::getTitleWithNumber() const
{
    if (this->copiesCreated > 0)
        return getTitle() + " " + QString::number(this->copiesCreated);
    else
        return getTitle();
}

Shader* CrtShader::createCopy() const
{
    this->copiesCreated++;
    return new CrtShader();
}
