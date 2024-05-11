
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
    scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(mainWidget);
    scrollArea->setVisible(false);

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

    auto connectSectionToShader = [&](Section* section, ShaderName shader)
    {
        // Connect checkbox
        connect(section, &Section::checkBoxStateChanged, glWidget,
            [this, shader](bool state)
            {
                glWidget->handleShaderToggled(state, shader);
            }
        );

        // Connect up button
        connect(section, &Section::buttonUpPressed, glWidget,
            [this, shader, section]()
            {
                if (moveSection(section, true))
                    glWidget->handleShaderMoveUp(shader);
            }
        );

        //  Connect down button
        connect(section, &Section::buttonDownPressed, glWidget,
            [this, shader, section]()
            {
                if (moveSection(section, false))
                    glWidget->handleShaderMoveDown(shader);
            }
        );

        // Connect copy button
        connect(section, &Section::buttonCopyPressed, glWidget,
            [this, shader]()
            {
                glWidget->handleShaderCopy(shader);
            }
        );

        // Connect remove button
        connect(section, &Section::buttonRemovePressed, glWidget,
            [this, shader]()
            {
                glWidget->handleShaderRemove(shader);
            }
        );
    };


    // COLOR CORRECTION
    Section* sectionCorrection = new Section("Color correction", 0, mainWidget);
    connectSectionToShader(sectionCorrection, ShaderName::Correction);
    QVBoxLayout* correctionLayout = new QVBoxLayout();

    // Exposure
    correctionLayout->addLayout(createLabelSlider(ShaderName::Correction,
                                                  CorrectionShader::exposureVals));
    // Contrast
    correctionLayout->addLayout(createLabelSlider(ShaderName::Correction,
                                                  CorrectionShader::contrastVals));

    // Temperature
    correctionLayout->addLayout(createLabelSlider(ShaderName::Correction,
                                                  CorrectionShader::temperatureVals));

    // Saturation
    correctionLayout->addLayout(createLabelSlider(ShaderName::Correction,
                                                  CorrectionShader::saturationVals));

    // Brightness
    correctionLayout->addLayout(createLabelSlider(ShaderName::Correction,
                                                  CorrectionShader::brightnessVals));

    // Color tint
    correctionLayout->addLayout(createLabelSlider(ShaderName::Correction,
                                                  CorrectionShader::tintIntensity));

    // Color filter
    correctionLayout->addLayout(createLabelSlider(ShaderName::Correction,
                                                  CorrectionShader::filterIntensity));

    sectionCorrection->setContentLayout(*correctionLayout);
    layout->addWidget(sectionCorrection);


    // SHARPNESS
    Section* sectionSharpness = new Section("Sharpness", 0, mainWidget);
    connectSectionToShader(sectionSharpness, ShaderName::Sharpness);
    QVBoxLayout* sharpnessLayout = new QVBoxLayout();

    // Strength
    sharpnessLayout->addLayout(createLabelSlider(ShaderName::Sharpness,
                                                  SharpnessShader::strengthVals));

    sectionSharpness->setContentLayout(*sharpnessLayout);
    layout->addWidget(sectionSharpness);


    // PIXELATE
    Section* sectionPixelate = new Section("Pixelate", 0, mainWidget);
    connectSectionToShader(sectionPixelate, ShaderName::Pixelate);
    QVBoxLayout* pixelateLayout = new QVBoxLayout();

    // Pixel size
    pixelateLayout->addLayout(createLabelSlider(ShaderName::Pixelate,
                                                PixelateShader::pixeSizeVals));

    sectionPixelate->setContentLayout(*pixelateLayout);
    layout->addWidget(sectionPixelate);


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
        scrollArea->setVisible(this->glWidget->loadTexture(fileName));
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

bool MainWindow::moveSection(QWidget* widget, bool moveUp)
{
    QVBoxLayout* parentLayout =
        qobject_cast<QVBoxLayout*>(widget->parentWidget()->layout());
    const int index = parentLayout->indexOf(widget);

    if (moveUp && index == 0)
        return false;
    if (!moveUp && index == parentLayout->count() - 1)
        return false;

    const int newIndex = moveUp ? index - 1 : index + 1;
    parentLayout->removeWidget(widget);
    parentLayout->insertWidget(newIndex , widget);

    return true;
}

QVBoxLayout* MainWindow::createLabelSlider(ShaderName shaderName,
    std::tuple<int, int, int, const char*, const char*> parameters)
{
    // LABEL

    QLineEdit* lineEdit = new QLineEdit(QString::number(std::get<2>(parameters)));
    lineEdit->setStyleSheet("QLineEdit {background: rgb(240, 240, 240);}");
    lineEdit->setFixedSize(20, 18);
    lineEdit->setAlignment(Qt::AlignCenter);
    lineEdit->setFrame(false);
    lineEdit->setValidator(new QIntValidator(
        std::get<0>(parameters),
        std::get<1>(parameters))
                           );

    QLabel* label = new QLabel(std::get<4>(parameters));
    label->setFixedWidth(100);

    QPushButton* resetButton = new QPushButton(this);
    QFont buttonFont = resetButton->font();
    buttonFont.setItalic(true);
    resetButton->setFont(buttonFont);
    resetButton->setText("reset");
    resetButton->setFixedSize(40, 19);
    resetButton->setStyleSheet("QPushButton {background: rgb(240, 240, 240);}");

    QHBoxLayout* horizLayoutLabel = new QHBoxLayout();
    horizLayoutLabel->setAlignment(Qt::AlignLeft);
    horizLayoutLabel->addWidget(label);
    horizLayoutLabel->addStretch();
    horizLayoutLabel->addWidget(lineEdit);
    horizLayoutLabel->addStretch();
    QSpacerItem* spacerLeft = new QSpacerItem(60, 20, QSizePolicy::Fixed,
                                              QSizePolicy::Fixed);
    horizLayoutLabel->addSpacerItem(spacerLeft);
    horizLayoutLabel->addWidget(resetButton);


    // SLIDER

    QSlider* slider = new QSlider(Qt::Orientation::Horizontal);
    slider->setMinimum(std::get<0>(parameters));
    slider->setMaximum(std::get<1>(parameters));
    slider->setValue(std::get<2>(parameters));

    QHBoxLayout* horizLayoutSlider = new QHBoxLayout();

    auto minLabel = new QLabel(QString::number(std::get<0>(parameters)));
    minLabel->setMinimumWidth(25);
    minLabel->setAlignment(Qt::AlignRight);
    auto maxLabel = new QLabel(QString::number(std::get<1>(parameters)));
    maxLabel->setMinimumWidth(25);
    maxLabel->setAlignment(Qt::AlignLeft);

    horizLayoutSlider->addWidget(minLabel);
    horizLayoutSlider->addWidget(slider);
    horizLayoutSlider->addWidget(maxLabel);

    const char* uniformName = std::get<3>(parameters);

    // CONNECT

    connect(slider, &QSlider::valueChanged, this,
            [this, shaderName, uniformName](int value)
            {
                this->glWidget->changeUniformValue
                    (value, shaderName, uniformName);
            });

    connect(slider, &QSlider::valueChanged, this,
            [this, lineEdit](int value)
            {
                lineEdit->setText(QString::number(value));
            });

    connect(lineEdit, &QLineEdit::textEdited, this,
            [this, slider](const QString &text)
            {
                slider->setValue(text.toInt());
            });

    connect(resetButton, &QPushButton::clicked, this,
            [this, slider, parameters]()
            {
                slider->setValue(std::get<2>(parameters));
            });

    // VBOX

    auto vbox = new QVBoxLayout();
    vbox->addLayout(horizLayoutLabel);
    vbox->addLayout(horizLayoutSlider);

    return vbox;
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
