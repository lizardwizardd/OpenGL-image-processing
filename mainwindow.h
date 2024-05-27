
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSlider>
#include <QHBoxLayout>
#include <QScrollArea>

#include "glwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

private slots:
    void chooseFile();
    void resizeToImage(int width, int height);
    void closeEvent(QCloseEvent *event);
private:
    GLWidget* glWidget;
    QWidget* mainWidget;
    QScrollArea* scrollArea;

    bool moveSection(QWidget* widget, bool moveUp);
    QSlider* createSlider(ShaderName shaderName, const Shader::ValueTuple& parameters);
    QHBoxLayout* createSliderLayout(ShaderName shaderName, const Shader::ValueTuple& parameters);
    QHBoxLayout* createLabelLayout(ShaderName shaderName, const Shader::ValueTuple& parameters);
    QVBoxLayout *createLabelSlider(ShaderName shaderName, const Shader::ValueTuple& parameters);
    QVBoxLayout *createLabelColorSelect(ShaderName shaderName, const Shader::ValueTuple& parameters);
    QVBoxLayout *createShaderParameters(ShaderName shaderName, const std::vector<Shader::ValueTuple> &parameters);
};

#endif // MAINWINDOW_H
