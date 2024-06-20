
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

public slots:
    void createShaderSettings(); // called when glWidget is initialized

private slots:
    void chooseFile();
    void resizeToImage(int width, int height);
    void closeEvent(QCloseEvent *event);

private:
    GLWidget* glWidget;
    QWidget* mainWidget;
    QScrollArea* scrollArea;
    QVBoxLayout* mainLayout; // settings layout

    bool moveSection(QWidget* widget, bool moveUp);
    QVBoxLayout* createLabelSlider(ShaderName shaderName, const Shader::ValueTuple& parameters);
    QVBoxLayout* createLabelColorSelect(ShaderName shaderName, const Shader::ValueTuple& parameters);
    QVBoxLayout* createShaderParameters(ShaderName shaderName, const std::vector<Shader::ValueTuple> &parameters);
};

#endif // MAINWINDOW_H
