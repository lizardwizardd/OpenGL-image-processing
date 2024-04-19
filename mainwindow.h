
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "glwidget.h"
#include "QSlider"

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

    QSlider* createSlider(ShaderName shaderName,
                          std::tuple<int, int, int, const char*> parameters);
};

#endif // MAINWINDOW_H
