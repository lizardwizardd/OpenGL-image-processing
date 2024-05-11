
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
    QSlider* createSlider(ShaderName shaderName,
        std::tuple<int, int, int, const char*, const char*> parameters);
    QHBoxLayout* createSliderLayout(ShaderName shaderName,
        std::tuple<int, int, int, const char*, const char*> parameters);
    QHBoxLayout* createLabelLayout(ShaderName shaderName,
                                   std::tuple<int, int, int, const char*, const char*> parameters);
    QVBoxLayout *createLabelSlider(ShaderName shaderName, std::tuple<int, int, int, const char *, const char *> parameters);
};

#endif // MAINWINDOW_H
