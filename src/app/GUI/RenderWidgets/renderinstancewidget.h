#ifndef RENDERINSTANCEWIDGET_H
#define RENDERINSTANCEWIDGET_H

#include "closablecontainer.h"
#include "renderinstancesettings.h"
#include "GUI/BoxesList/boxsinglewidget.h"
#include <QPushButton>
#include <QMouseEvent>
#include <QAction>
#include <QMenu>
class OutputSettingsDisplayWidget;
class RenderInstanceWidget;
class OutputProfilesListButton : public QPushButton {
    Q_OBJECT
public:
    OutputProfilesListButton(RenderInstanceWidget *parent);
signals:
    void profileSelected(OutputSettingsProfile *);
protected:
    void mousePressEvent(QMouseEvent *e);
    RenderInstanceWidget *mParentWidget;
};

class RenderInstanceWidget : public ClosableContainer {
    Q_OBJECT
public:
    RenderInstanceWidget(QWidget *parent = nullptr);
    RenderInstanceWidget(RenderInstanceSettings *settings,
                         QWidget *parent = nullptr);
    ~RenderInstanceWidget();

    RenderInstanceSettings *getSettings();
private:
    ClosableContainer *mOutputSettings;
    OutputSettingsDisplayWidget *mOutputSettingsDisplayWidget;

    QPushButton *mOutputDestinationButton;

    OutputProfilesListButton *mOutputSettingsProfilesButton;
    QPushButton *mOutputSettingsButton;
    QLabel *mNameLabel;
    QVBoxLayout *mContentLayout = new QVBoxLayout();
    RenderInstanceSettings *mSettings;
signals:

protected:
    void updateOutputDestinationFromCurrentFormat();
private slots:
    void updateFromSettings();

    void outputSettingsProfileSelected(OutputSettingsProfile *profile);

    void openOutputSettingsDialog();
    void openOutputDestinationDialog();
    void openRenderSettingsDialog();
};

#endif // RENDERINSTANCEWIDGET_H