
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
#include <QLineEdit>
#include <QIntValidator>


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

    // COLOR CORRECTION
    Section* sectionCorrection = new Section("Color correction", 0, mainWidget);
    QVBoxLayout* correctionLayout = new QVBoxLayout();

    // Exposure
    correctionLayout->addLayout(createLabelLayout(ShaderName::Correction,
                                                  CorrectionShader::exposureVals));
    correctionLayout->addLayout(createSliderLayout(ShaderName::Correction,
                                                   CorrectionShader::exposureVals));

    // Contrast
    correctionLayout->addLayout(createLabelLayout(ShaderName::Correction,
                                                  CorrectionShader::contrastVals));
    correctionLayout->addLayout(createSliderLayout(ShaderName::Correction,
                                             CorrectionShader::contrastVals));

    // Temperature
    correctionLayout->addLayout(createLabelLayout(ShaderName::Correction,
                                                  CorrectionShader::temperatureVals));
    correctionLayout->addLayout(createSliderLayout(ShaderName::Correction,
                                             CorrectionShader::temperatureVals));;

    // Saturation
    correctionLayout->addLayout(createLabelLayout(ShaderName::Correction,
                                                  CorrectionShader::saturationVals));
    correctionLayout->addLayout(createSliderLayout(ShaderName::Correction,
                                             CorrectionShader::saturationVals));

    // Brightness
    correctionLayout->addLayout(createLabelLayout(ShaderName::Correction,
                                                  CorrectionShader::brightnessVals));
    correctionLayout->addLayout(createSliderLayout(ShaderName::Correction,
                                             CorrectionShader::brightnessVals));

    sectionCorrection->setContentLayout(*correctionLayout);
    layout->addWidget(sectionCorrection);

    // SHARPNESS
    Section* sectionSharpness = new Section("Sharpness", 0, mainWidget);
    QVBoxLayout* sharpnessLayout = new QVBoxLayout();

    // Strength
    sharpnessLayout->addLayout(createLabelLayout(ShaderName::Sharpness,
                                                  SharpnessShader::strengthVals));
    sharpnessLayout->addWidget(createSlider(ShaderName::Sharpness,
                                            SharpnessShader::strengthVals));

    sectionSharpness->setContentLayout(*sharpnessLayout);
    layout->addWidget(sectionSharpness);

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

QHBoxLayout* MainWindow::createLabelLayout(ShaderName shaderName,
               std::tuple<int, int, int, const char*, const char*> parameters)
{
    QLineEdit* lineEdit = new QLineEdit(QString::number(std::get<2>(parameters)));
    lineEdit->setStyleSheet("QLineEdit {background: rgb(240, 240, 240);}");
    lineEdit->setMaximumWidth(30);
    lineEdit->setMaximumHeight(20);
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setFrame(false);
    lineEdit->setValidator(new QIntValidator(
        std::get<0>(parameters),
        std::get<1>(parameters))
    );
    // TODO connect lineEdit

    QLabel* label = new QLabel(std::get<4>(parameters));
    label->setMinimumWidth(110);

    QHBoxLayout* horizLayout = new QHBoxLayout();
    horizLayout->setAlignment(Qt::AlignLeft);
    horizLayout->addWidget(label, 0, Qt::AlignLeft);
    horizLayout->addWidget(lineEdit, 0, Qt::AlignLeft);

    return horizLayout;
}

QHBoxLayout* MainWindow::createSliderLayout(ShaderName shaderName,
                std::tuple<int, int, int, const char*, const char*> parameters)
{
    QSlider* slider = new QSlider(Qt::Orientation::Horizontal);
    slider->setMinimum(std::get<0>(parameters));
    slider->setMaximum(std::get<1>(parameters));
    slider->setValue(std::get<2>(parameters));

    QHBoxLayout* horizLayout = new QHBoxLayout();

    auto minLabel = new QLabel(QString::number(std::get<0>(parameters)));
    minLabel->setMinimumWidth(25);
    minLabel->setAlignment(Qt::AlignRight);
    auto maxLabel = new QLabel(QString::number(std::get<1>(parameters)));
    maxLabel->setMinimumWidth(25);
    maxLabel->setAlignment(Qt::AlignLeft);

    horizLayout->addWidget(minLabel);
    horizLayout->addWidget(slider);
    horizLayout->addWidget(maxLabel);

    const char* uniformName = std::get<3>(parameters);

    connect(slider, &QSlider::valueChanged, this,
            [this, shaderName, uniformName](int value)
            {
                this->glWidget->changeUniformValue
                    (value, shaderName, uniformName);
            });

    return horizLayout;
}


QSlider* MainWindow::createSlider(ShaderName shaderName,
      std::tuple<int, int, int, const char*, const char*> parameters)
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
