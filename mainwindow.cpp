
#include "mainwindow.h"

#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFileDialog>


MainWindow::MainWindow()
{
    QMenuBar* menuBar = new QMenuBar;
    QMenu* menuList = menuBar->addMenu("File");

    QAction* openFile = new QAction(menuList);
    openFile->setText("Open file");
    menuList->addAction(openFile);
    connect(openFile, &QAction::triggered, this, &MainWindow::chooseFile);
    setMenuBar(menuBar);

    glWidget = new GLWidget(this);
    connect(glWidget, &GLWidget::imageSizeChanged, this, &MainWindow::resizeToImage);
    setCentralWidget(glWidget);
}

void MainWindow::chooseFile()
{
    QString defaultImgDir = QStandardPaths::writableLocation
        (QStandardPaths::PicturesLocation);

    QString fileName = QFileDialog::getOpenFileName(this, "Choose an Image",
                       defaultImgDir, "Images (*.png *.jpg *.bmp)");

    if (!fileName.isEmpty())
    {
        this->glWidget->loadTexture(fileName);
    }
}

void MainWindow::resizeToImage(int width, int height)
{
    QSize newSize(width, height); // TODO: auto margin
    this->resize(newSize);
}
