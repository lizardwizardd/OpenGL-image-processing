
#ifndef SECTION_H
#define SECTION_H

#include <QFrame>
#include <QGridLayout>
#include <QParallelAnimationGroup>
#include <QScrollArea>
#include <QToolButton>
#include <QCheckBox>
#include <QWidget>

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

public slots:
    void toggle(bool collapsed);
    void checkBoxStateChangedSlot(int state);

public:
    static const int DEFAULT_DURATION = 0;

    // Initialize section
    explicit Section(const QString& title = "", const int animationDuration = DEFAULT_DURATION, QWidget* parent = 0);

    void setContentLayout(QLayout& contentLayout);
    void setTitle(QString title);
    void updateHeights();
};


#endif // SECTION_H
