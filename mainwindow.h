
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

private slots:
    void chooseFile();
    void resizeToImage(int width, int height);
private:
    GLWidget* glWidget;
};

#endif // MAINWINDOW_H
