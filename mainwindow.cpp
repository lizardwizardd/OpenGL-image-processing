
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
    correctionLayout->addWidget(createSlider(ShaderName::Correction,
                                             CorrectionShader::exposureVals));

    // Contrast
    correctionLayout->addWidget(new QLabel("Contrast", sectionCorrection));
    correctionLayout->addWidget(createSlider(ShaderName::Correction,
                                             CorrectionShader::contrastVals));

    // Temperature
    correctionLayout->addWidget(new QLabel("Temperature", sectionCorrection));
    correctionLayout->addWidget(createSlider(ShaderName::Correction,
                                             CorrectionShader::temperatureVals));;

    // Saturation
    correctionLayout->addWidget(new QLabel("Saturation", sectionCorrection));
    correctionLayout->addWidget(createSlider(ShaderName::Correction,
                                             CorrectionShader::saturationVals));

    // Brightness
    correctionLayout->addWidget(new QLabel("Brightness", sectionCorrection));
    correctionLayout->addWidget(createSlider(ShaderName::Correction,
                                             CorrectionShader::brightnessVals));

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

QSlider *MainWindow::createSlider(ShaderName shaderName,
      std::tuple<int, int, int, const char*> parameters)
{
    QSlider* slider = new QSlider(Qt::Orientation::Horizontal, this);
    slider->setMinimum(std::get<0>(parameters));
    slider->setMaximum(std::get<1>(parameters));
    slider->setValue(std::get<2>(parameters));

    const char* uniformName = std::get<3>(parameters);

    connect(slider, &QSlider::valueChanged, this,
            [this, shaderName, uniformName](int value)
            {
                this->glWidget->changeUniformValue
                    (value, shaderName, uniformName);
            });

    return slider;
}
