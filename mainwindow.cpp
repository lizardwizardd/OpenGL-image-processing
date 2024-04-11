
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
    glWidget->setMinimumSize(QSize(300, 300));
    //glWidget->setMaximumSize(QSize(1800, 1000));
    connect(this, &MainWindow::destroyed, glWidget, &GLWidget::close);
    connect(glWidget, &GLWidget::destroyed, this, &MainWindow::close);
}

MainWindow::~MainWindow()
{
    delete glWidget;
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (glWidget)
        glWidget->close();
    event->accept();
}
