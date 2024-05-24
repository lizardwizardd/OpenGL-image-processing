
#include <QPropertyAnimation>

#include "Section.h"
#include <QDebug>


Section::Section(const QString& title, const int animationDuration, QWidget* parent)
    : QWidget(parent), animationDuration(animationDuration)
{
    checkBox = new QCheckBox(this);
    toggleButton = new QToolButton(this);
    headerLine = new QFrame(this);
    QPushButton* upButton = new QPushButton(this);
    QPushButton* downButton = new QPushButton(this);
    QPushButton* copyButton = new QPushButton(this);
    // Not visible by default, appears when there's a copy
    QPushButton* removeButton = new QPushButton(this);
    toggleAnimation = new QParallelAnimationGroup(this);
    contentArea = new QScrollArea(this);
    mainLayout = new QGridLayout(this);

    toggleButton->setStyleSheet("QToolButton {border: none;}");
    toggleButton->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toggleButton->setArrowType(Qt::ArrowType::RightArrow);
    toggleButton->setIconSize(QSize(10, 10));
    toggleButton->setText(title);
    toggleButton->setCheckable(true);
    toggleButton->setChecked(false);

    headerLine->setFrameShape(QFrame::HLine);
    headerLine->setFrameShadow(QFrame::Sunken);
    headerLine->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);

    upButton->setText("U");
    upButton->setFixedSize(18, 18);
    upButton->setToolTip("Move up in order");
    upButton->setStyleSheet("QPushButton {background: rgb(240, 240, 240);}");
    downButton->setText("D");
    downButton->setFixedSize(18, 18);
    downButton->setToolTip("Move down in order");
    downButton->setStyleSheet("QPushButton {background: rgb(240, 240, 240);}");
    copyButton->setText("C");
    copyButton->setVisible(false); // todo
    copyButton->setFixedSize(18, 18);
    copyButton->setToolTip("Copy");
    copyButton->setStyleSheet("QPushButton {background: rgb(240, 240, 240);}");
    removeButton->setVisible(false); // todo
    removeButton->setText("R");
    removeButton->setFixedSize(18, 18);
    removeButton->setToolTip("Remove");
    removeButton->setStyleSheet("QPushButton {background: rgb(240, 240, 240);}");

    contentArea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    // Start out collapsed
    contentArea->setMaximumHeight(0);
    contentArea->setMinimumHeight(0);

    // Let the entire widget grow and shrink with its content
    toggleAnimation->addAnimation(new QPropertyAnimation(this, "maximumHeight"));
    toggleAnimation->addAnimation(new QPropertyAnimation(this, "minimumHeight"));
    toggleAnimation->addAnimation(new QPropertyAnimation(contentArea, "maximumHeight"));

    mainLayout->setVerticalSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    int row = 0;
    mainLayout->setHorizontalSpacing(3);
    mainLayout->addWidget(checkBox, 0, 0, 1, 1, Qt::AlignLeft);
    mainLayout->addWidget(toggleButton, 0, 1, 1, 1, Qt::AlignLeft);
    mainLayout->addWidget(headerLine, row, 2, 1, 1);

    // To the right of the header line
    mainLayout->addWidget(upButton, row, 3);
    mainLayout->addWidget(downButton, row, 4);
    mainLayout->addWidget(copyButton, row, 5);
    mainLayout->addWidget(removeButton, row, 6);

    mainLayout->setColumnStretch(2, 1);
    mainLayout->setColumnStretch(3, 0);
    mainLayout->setColumnStretch(4, 0);
    mainLayout->setColumnStretch(5, 0);
    mainLayout->setColumnStretch(6, 0);

    mainLayout->addWidget(contentArea, ++row, 0, 1, 7); // Increment row to place content below buttons
    setLayout(mainLayout);

    connect(toggleButton, &QToolButton::toggled, this, &Section::toggle);
    connect(checkBox, &QCheckBox::stateChanged, this, &Section::checkBoxStateChangedSlot);
    connect(upButton, &QPushButton::clicked, this, &Section::buttonUpPressedSlot);
    connect(downButton, &QPushButton::clicked, this, &Section::buttonDownPressedSlot);
    connect(copyButton, &QPushButton::clicked, this, &Section::buttonCopyPressedSlot);
    connect(removeButton, &QPushButton::clicked, this, &Section::buttonRemovePressedSlot);
}


void Section::checkBoxStateChangedSlot(int state)
{
    emit checkBoxStateChanged(state == Qt::Checked);
}

void Section::buttonUpPressedSlot(bool state)
{
    emit buttonUpPressed();
}

void Section::buttonDownPressedSlot(bool state)
{
    emit buttonDownPressed();
}

void Section::buttonCopyPressedSlot(bool state)
{
    emit buttonCopyPressed();
}

void Section::buttonRemovePressedSlot(bool state)
{
    emit buttonRemovePressed();
}

void Section::toggle(bool expanded)
{
    toggleButton->setArrowType(expanded ? Qt::ArrowType::DownArrow : Qt::ArrowType::RightArrow);
    toggleAnimation->setDirection(expanded ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    toggleAnimation->start();

    this->isExpanded = expanded;

    qDebug() << "MV: toggle: isExpanded " << isExpanded;
}

void Section::setContentLayout(QLayout& contentLayout)
{
    delete contentArea->layout();
    contentArea->setLayout(&contentLayout);
    collapsedHeight = sizeHint().height() - contentArea->maximumHeight();

    updateHeights();
}

void Section::setTitle(QString title)
{
    toggleButton->setText(std::move(title));
}

void Section::setNotExpandable()
{
    toggleButton->setEnabled(false);
    toggleButton->setToolButtonStyle(Qt::ToolButtonTextOnly);
}

void Section::updateHeights()
{
    int contentHeight = contentArea->layout()->sizeHint().height();

    for (int i = 0; i < toggleAnimation->animationCount() - 1; ++i)
    {
        QPropertyAnimation* SectionAnimation = static_cast<QPropertyAnimation *>(toggleAnimation->animationAt(i));
        SectionAnimation->setDuration(animationDuration);
        SectionAnimation->setStartValue(collapsedHeight);
        SectionAnimation->setEndValue(collapsedHeight + contentHeight);
    }

    QPropertyAnimation* contentAnimation = static_cast<QPropertyAnimation *>(toggleAnimation->animationAt(toggleAnimation->animationCount() - 1));
    contentAnimation->setDuration(animationDuration);
    contentAnimation->setStartValue(0);
    contentAnimation->setEndValue(contentHeight);

    toggleAnimation->setDirection(isExpanded ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    toggleAnimation->start();
}
