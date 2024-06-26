
#ifndef SECTION_H
#define SECTION_H

#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QCheckBox>
#include <QWidget>
#include <QPushButton>

class Section : public QWidget
{
    Q_OBJECT

private:
    QGridLayout* mainLayout;
    QToolButton* toggleButton;
    QFrame* headerLine;
    QParallelAnimationGroup* toggleAnimation;
    QScrollArea* contentArea;
    QCheckBox* checkBox;
    int animationDuration;
    int collapsedHeight;
    bool isExpanded = false;

signals:
    void checkBoxStateChanged(bool checked);
    void buttonUpPressed();
    void buttonDownPressed();
    void buttonCopyPressed();
    void buttonRemovePressed();

public slots:
    void toggle(bool collapsed);
    void checkBoxStateChangedSlot(int state);
    void buttonUpPressedSlot(bool state);
    void buttonDownPressedSlot(bool state);
    void buttonCopyPressedSlot(bool state);
    void buttonRemovePressedSlot(bool state);

public:
    static const int DEFAULT_DURATION = 0;

    // Initialize section
    explicit Section(const QString& title = "", const int animationDuration = DEFAULT_DURATION, QWidget* parent = 0);

    void setContentLayout(QLayout& contentLayout);
    void setTitle(QString title);
    void setNotExpandable();
    void updateHeights();
};


#endif // SECTION_H
