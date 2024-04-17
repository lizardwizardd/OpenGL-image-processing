
#include "mainwindow.h"
#include "section.h"

#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFileDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>


MainWindow::MainWindow()
{
    // Window setup
    this->setWindowTitle("Image Processor");
    this->resize(300, 500);
    this->setMinimumWidth(300);

    // Menu bar setup
    QMenuBar* menuBar = new QMenuBar;
    QMenu* menuList = menuBar->addMenu("File");
    QAction* openFile = new QAction(menuList);
    openFile->setText("Open file");
    menuList->addAction(openFile);
    connect(openFile, &QAction::triggered, this, &MainWindow::chooseFile);
    setMenuBar(menuBar);

    // Main widget
    mainWidget = new QWidget(this);

    // Scroll Area
    QScrollArea* scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(mainWidget);

    // Main layout
    QVBoxLayout* layout = new QVBoxLayout(mainWidget);
    layout->setAlignment(Qt::AlignTop);

    // GLWidget setup
    glWidget = new GLWidget(this);
    glWidget->setMinimumSize(QSize(300, 300));
    connect(this, &MainWindow::destroyed, glWidget, &GLWidget::close);
    connect(glWidget, &GLWidget::destroyed, this, &MainWindow::close);

    // ---------------------
    // SHADER SETTINGS SETUP
    // ---------------------

    // Color correction
    Section* sectionCorrection = new Section("Color correction", 0, mainWidget);
    QVBoxLayout* correctionLayout = new QVBoxLayout();

    // TODO shader class (?)

    // Exposure
    correctionLayout->addWidget(new QLabel("Exposure", sectionCorrection));
    correctionLayout->addWidget(createSlider(-200, 200, 0, ShaderName::
                                             Correction, (char*)"exposure"));

    // Contrast
    correctionLayout->addWidget(new QLabel("Contrast", sectionCorrection));
    correctionLayout->addWidget(createSlider(0, 200, 100, ShaderName::
                                             Correction, (char*)"contrast"));

    // Temperature
    correctionLayout->addWidget(new QLabel("Temperature", sectionCorrection));
    correctionLayout->addWidget(createSlider(0, 100, 0, ShaderName::
                                             Correction, (char*)"temperature"));

    // Saturation
    correctionLayout->addWidget(new QLabel("Saturation", sectionCorrection));
    correctionLayout->addWidget(createSlider(0, 200, 100, ShaderName::
                                             Correction, (char*)"saturation"));

    // Brightness
    correctionLayout->addWidget(new QLabel("Brightness", sectionCorrection));
    correctionLayout->addWidget(createSlider(-100, 100, 0, ShaderName::
                                             Correction, (char*)"brightness"));

    sectionCorrection->setContentLayout(*correctionLayout);

    layout->addWidget(sectionCorrection);

    // Set scroll area as central widget
    this->setCentralWidget(scrollArea);
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
        this->glWidget->loadTexture(fileName);
    else
        qDebug() << "Can't load file: fileName is empty";
}

void MainWindow::resizeToImage(int width, int height)
{
    QSize newSize(width, height);
    this->resize(newSize);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (glWidget)
        glWidget->close();
    event->accept();
}

QSlider *MainWindow::createSlider(int minValue, int maxValue, int defaultValue,
                                  ShaderName shaderName, char *uniformName)
{
    QSlider* slider = new QSlider(Qt::Orientation::Horizontal, this);
    slider->setMinimum(minValue);
    slider->setMaximum(maxValue);
    slider->setValue(defaultValue);

    connect(slider, &QSlider::valueChanged, this,
            [this, shaderName, uniformName](int value)
            {
                this->glWidget->changeUniformValue
                    (value, shaderName, uniformName);
            });

    return slider;
}
